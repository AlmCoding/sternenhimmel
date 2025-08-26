import asyncio
import CmdBuilder as cb
import DaisyChain as dc
import BleClient as bc


class ConfigTool:
    def __init__(self):
        self.chain = None
        self.client = None
        self.rid_counter = 0
        self.uploaded = False
        self.verified = False
        self.uploaded_leds = []

    async def load(self, config_file_path: str) -> bool:
        self.chain = dc.DaisyChain()
        self.chain.load_config(config_file_path)

        self.uploaded = False
        self.verified = False
        return True

    async def connect(self) -> bool:
        if not self.client:
            self.client = bc.BleClient()
        success = await self.client.connect()
        return success

    async def disconnect(self) -> bool:
        success = True
        if self.client:
            success = await self.client.disconnect()
            self.client = None
        return success

    def _get_changed_leds(self) -> list[dc.Led]:
        if not self.chain:
            raise RuntimeError("ConfigTool not properly initialized. Call 'load' first!")

        leds = self.chain.get_leds()  # config file LEDs
        uploaded_leds = self.uploaded_leds  # previously uploaded LEDs
        changed_leds = []

        if len(uploaded_leds) != len(leds):
            assert len(uploaded_leds) == 0
            return leds  # First time upload, return all LEDs

        for idx, led in enumerate(leds):
            uploaded_led = uploaded_leds[idx]
            assert led.pcb_index == uploaded_led.pcb_index and led.led_index == uploaded_led.led_index

            if led.brightness != uploaded_led.brightness:
                print(
                    f"\tLED({led.pcb_index:02d},{led.led_index:02d}) change detected: {uploaded_led.brightness:03d} -> {led.brightness:03d}"
                )
                changed_leds.append(led)

        return changed_leds

    async def upload(self) -> bool:
        if not self.chain or not self.client:
            raise RuntimeError("ConfigTool not properly initialized. Call 'load' and 'connect' first!")

        leds = self._get_changed_leds()
        if len(leds) == 0:
            print("No LED changes detected, skipping upload.")
            self.uploaded = True
            self.verified = False
            return True

        print(f"Starting upload of {len(leds)} LEDs ...")
        chunk_size = dc.LED_TOTAL // 12
        leds_list = [leds[i : i + chunk_size] for i in range(0, len(leds), chunk_size)]

        for i, leds in enumerate(leds_list):
            print(f"Uploading chunk {i+1}/{len(leds_list)} with {len(leds)} LEDs ...")
            success = await self._upload_leds(leds)
            if not success:
                print("Failed to upload LED chunk!")
                self.uploaded_leds = []
                return False

        print("All LEDs uploaded successfully.")
        self.uploaded_leds = self.chain.get_leds()
        self.uploaded = True
        self.verified = False
        return True

    async def _upload_leds(self, leds: list[dc.Led]) -> bool:
        self.rid_counter += 1
        cmd = cb.CmdBuilder.set_brightness(rid=self.rid_counter, leds=leds)
        response = await self.client.send_command(cmd, timeout=5.0)
        status = cb.CmdBuilder.evaluate_set_brightness_response(response, rid=self.rid_counter)
        return status

    async def verify(self) -> bool:
        if not self.chain or not self.client or not self.uploaded:
            raise RuntimeError(
                "ConfigTool not properly initialized or LEDs not uploaded. Call 'load', 'connect' and 'upload' first!"
            )

        print("Verifying uploaded LEDs ...")
        leds = await self.download()
        if not leds:
            print("Failed to download LEDs for verification!")
            return False

        for idx, led in enumerate(leds):
            expected_led = self.chain.leds[idx]
            if (
                led.pcb_index != expected_led.pcb_index
                or led.led_index != expected_led.led_index
                or led.brightness != expected_led.brightness
            ):
                print(f"Error: LED data mismatch for LED ({idx}): expected {expected_led}, got {led}")
                return False
        print("All LEDs verified successfully.")
        self.verified = True
        return True

    async def download(self) -> list[dc.Led] | None:
        if not self.chain or not self.client:
            raise RuntimeError("ConfigTool not properly initialized. Call 'load', 'connect' and 'upload' first!")

        print(f"Starting download of {dc.LED_TOTAL} LEDs ...")
        downloaded_leds = []
        leds = self.chain.get_leds()
        chunk_size = dc.LED_TOTAL // 12
        leds_list = [leds[i : i + chunk_size] for i in range(0, len(leds), chunk_size)]

        for i, leds in enumerate(leds_list):
            print(f"Downloading chunk {i+1}/{len(leds_list)} with {len(leds)} LEDs ...")
            downloaded = await self._download_leds(leds)
            if not downloaded:
                print("Failed to download LED chunk!")
                return None
            downloaded_leds += leds

        print("All LEDs downloaded successfully.")
        return downloaded_leds

    async def _download_leds(self, leds: list[dc.Led]) -> list[dc.Led] | None:
        self.rid_counter += 1
        cmd = cb.CmdBuilder.get_brightness(rid=self.rid_counter, leds=leds)
        response = await self.client.send_command(cmd, timeout=5.0)
        downloaded_leds = cb.CmdBuilder.evaluate_get_brightness_response(response, rid=self.rid_counter)
        return downloaded_leds

    async def save(self) -> bool:
        if not self.chain or not self.client or not self.uploaded:
            raise RuntimeError(
                "ConfigTool not properly initialized or LEDs not uploaded. Call 'load', 'connect' and 'upload' first!"
            )

        self.rid_counter += 1
        cmd = cb.CmdBuilder.save_calibration(rid=self.rid_counter, name=self.chain.name)
        response = await self.client.send_command(cmd, timeout=3.0)

        status = cb.CmdBuilder.evaluate_save_calibration_response(response, rid=self.rid_counter)
        if not status:
            print("Failed to save calibration on device!")
            return False
        print("Calibration saved successfully on device.")
        return True


async def main():
    config_tool = ConfigTool()
    await config_tool.load("chain_config.json")
    await config_tool.connect()

    if await config_tool.upload():
        if await config_tool.verify():
            await config_tool.save()

    await config_tool.disconnect()


if __name__ == "__main__":
    asyncio.run(main())
