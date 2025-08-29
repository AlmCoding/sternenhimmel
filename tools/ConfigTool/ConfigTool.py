import asyncio
import CmdBuilder as cb
import DaisyChain as dc
import BleClient as bc
from helper import print_pretty_json
import datetime
import copy
import json


class ConfigTool:
    def __init__(self):
        self.chain = dc.DaisyChain()
        self.client = None
        self.rid_counter = 0
        self.verified = False
        self.device_leds = []  # uploaded or downloaded LEDs from device
        self.cmd_file_data = bytearray()
        self.cmd_file_rid = 0

    async def load(self, config_file_path: str) -> bool:
        self.chain.load_config(config_file_path)
        self.verified = False
        return True

    async def load_cmd(self, cmd_file_path: str) -> bool:
        try:
            with open(cmd_file_path, "r", encoding="utf-8") as f:
                doc = json.load(f)
        except Exception as e:
            raise RuntimeError(f"Failed to load command file '{cmd_file_path}': {e}")

        if "rid" not in doc or not isinstance(doc["rid"], int):
            raise ValueError("Invalid cmd file: 'rid' key missing or not an integer")
        if "cmd" not in doc or not isinstance(doc["cmd"], str):
            raise ValueError("Invalid cmd file: 'cmd' key missing or not a string")

        print_pretty_json(doc)
        self.cmd_file_rid = doc["rid"]

        self.cmd_file_data = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        print(f"Loaded command file: '{cmd_file_path}' and null-terminated it for BLE transmission.")
        return True

    async def connect(self) -> bool:
        if not self.client:
            self.client = bc.BleClient()
        success = await self.client.connect()
        if not success:
            self.client = None
        return success

    async def disconnect(self) -> bool:
        success = True
        if self.client:
            success = await self.client.disconnect()
            self.client = None
        return success

    async def get_info(self) -> tuple[str, str] | None:
        if not self.client:
            raise RuntimeError("ConfigTool not properly initialized. Call 'connect' first!")

        self.rid_counter += 1
        cmd = cb.CmdBuilder.get_version(rid=self.rid_counter)
        response = await self.client.send_command(cmd, timeout=3.0)
        version = cb.CmdBuilder.evaluate_get_version_response(response, rid=self.rid_counter)
        if not version:
            print("Failed to get device version!")
            return None

        self.rid_counter += 1
        cmd = cb.CmdBuilder.get_calibration_name(rid=self.rid_counter)
        response = await self.client.send_command(cmd, timeout=3.0)
        name = cb.CmdBuilder.evaluate_get_calibration_name_response(response, rid=self.rid_counter)
        if not name:
            print("Failed to get device calibration name!")
            return None

        print("Device info:")
        print(f"\tFirmware version: '{version}'")
        print(f"\tCalibration name: '{name}'")
        return version, name

    async def send_cmd(self) -> bool:
        if not self.client:
            raise RuntimeError("ConfigTool not properly initialized. Call 'connect' first!")
        if len(self.cmd_file_data) == 0:
            raise RuntimeError("No command loaded. Call 'load_cmd' first!")

        response = await self.client.send_command(self.cmd_file_data, timeout=3.0)
        success, _ = cb.CmdBuilder._evaluate_response(response, rid=self.cmd_file_rid, status=0)
        if not success:
            print("Command execution failed!")
            return False
        return True

    async def stop_show(self) -> bool:
        if not self.client:
            raise RuntimeError("ConfigTool not properly initialized. Call 'connect' first!")

        self.rid_counter += 1
        cmd = cb.CmdBuilder.stop_show(rid=self.rid_counter)
        response = await self.client.send_command(cmd, timeout=3.0)
        status = cb.CmdBuilder.evaluate_stop_show_response(response, rid=self.rid_counter)
        if status:
            print("Show stopped successfully on device.")
        else:
            print("Failed to stop show on device!")
        return status

    def _get_changed_leds(self) -> list[dc.Led]:
        if not self.chain:
            raise RuntimeError("ConfigTool not properly initialized. Call 'load' first!")

        config_leds = self.chain.leds  # config file LEDs
        device_leds = self.device_leds  # previously uploaded or downloaded LEDs from device
        changed_leds = []

        if len(device_leds) != len(config_leds):
            assert len(device_leds) == 0
            return config_leds  # First time upload, return all LEDs

        for idx, led in enumerate(config_leds):
            device_led = device_leds[idx]
            assert led.pcb_index == device_led.pcb_index and led.led_index == device_led.led_index

            if led.brightness != device_led.brightness:
                print(
                    f"\tLED({led.pcb_index:02d},{led.led_index:02d}) change detected: {device_led.brightness:03d} -> {led.brightness:03d}"
                )
                changed_leds.append(led)
        return changed_leds

    async def upload(self) -> bool:
        if not self.chain or not self.client:
            raise RuntimeError("ConfigTool not properly initialized. Call 'load' and 'connect' first!")

        leds = self._get_changed_leds()
        if len(leds) == 0:
            print("No LED changes detected, skipping upload.")
            return True

        print(f"Starting upload of {len(leds)} LEDs ...")
        self.verified = False  # Reset verified flag on changes

        chunk_size = dc.LED_TOTAL // 12
        leds_list = [leds[i : i + chunk_size] for i in range(0, len(leds), chunk_size)]

        for i, leds in enumerate(leds_list):
            print(f"Uploading chunk {i+1}/{len(leds_list)} with {len(leds)} LEDs ...")
            success = await self._upload_leds(leds)
            if not success:
                print("Failed to upload LED chunk!")
                self.device_leds = []
                return False

        print("All LEDs uploaded successfully.")
        self.device_leds = copy.deepcopy(self.chain.leds)  # Store uploaded LEDs for future change detection
        return True

    async def _upload_leds(self, leds: list[dc.Led]) -> bool:
        self.rid_counter += 1
        cmd = cb.CmdBuilder.set_brightness(rid=self.rid_counter, leds=leds)
        response = await self.client.send_command(cmd, timeout=5.0)
        status = cb.CmdBuilder.evaluate_set_brightness_response(response, rid=self.rid_counter)
        return status

    async def verify(self) -> bool:
        if not self.chain or not self.client:
            raise RuntimeError("ConfigTool not properly initialized. Call 'load' and 'connect' first!")

        print("Verifying device state against config file ...")
        self.verified = False

        if not await self.download():
            print("Failed to download LEDs for verification!")
            return False

        mismatch_count = 0
        for idx, led in enumerate(self.device_leds):
            expected_led = self.chain.leds[idx]
            if (
                led.pcb_index != expected_led.pcb_index
                or led.led_index != expected_led.led_index
                or led.brightness != expected_led.brightness
            ):
                print(
                    f"\tMismatch for LED({led.pcb_index},{led.led_index}): expected {expected_led.brightness}, got {led.brightness}"
                )
                mismatch_count += 1

        if mismatch_count > 0:
            print(f"LED verification detected {mismatch_count} mismatche(s)!")
            return False

        print("All LEDs verified successfully.")
        self.verified = True
        return True

    async def download(self) -> bool:
        if not self.chain or not self.client:
            raise RuntimeError("ConfigTool not properly initialized. Call 'load' and 'connect' first!")

        print(f"Starting download of {dc.LED_TOTAL} LEDs ...")
        config_leds = self.chain.leds  # config file LEDs
        chunk_size = dc.LED_TOTAL // 12
        leds_list = [config_leds[i : i + chunk_size] for i in range(0, len(config_leds), chunk_size)]
        downloaded_leds = []

        for i, leds in enumerate(leds_list):
            print(f"Downloading chunk {i+1}/{len(leds_list)} with {len(leds)} LEDs ...")
            downloaded_chunk = await self._download_leds(leds)
            if not downloaded_chunk:
                print("Failed to download LED chunk!")
                self.device_leds = []
                return False
            downloaded_leds += downloaded_chunk

        print("All LEDs downloaded successfully.")
        self.device_leds = downloaded_leds  # Store downloaded LEDs for future change detection
        return True

    async def _download_leds(self, leds: list[dc.Led]) -> list[dc.Led] | None:
        self.rid_counter += 1
        cmd = cb.CmdBuilder.get_brightness(rid=self.rid_counter, leds=leds)
        response = await self.client.send_command(cmd, timeout=5.0)
        downloaded_leds = cb.CmdBuilder.evaluate_get_brightness_response(response, rid=self.rid_counter)
        return downloaded_leds

    async def save(self) -> bool:
        if not self.chain or not self.client or not self.verified:
            raise RuntimeError(
                "ConfigTool not properly initialized. Call 'load', 'connect', 'upload' and 'verify' first!"
            )
        name = f"{self.chain.name} {datetime.datetime.now():%Y/%m/%d %H:%M}"
        print(f"Saving calibration on device with name: '{name}' ...")

        self.rid_counter += 1
        cmd = cb.CmdBuilder.save_calibration(rid=self.rid_counter, name=name)
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
    await config_tool.get_info()

    if await config_tool.upload():
        if await config_tool.verify():
            await config_tool.save()
            await config_tool.get_info()

    await config_tool.disconnect()


if __name__ == "__main__":
    asyncio.run(main())
