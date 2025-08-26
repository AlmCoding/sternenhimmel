import json
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

    def load(self, config_file_path: str):
        self.chain = dc.DaisyChain()
        self.chain.load_config(config_file_path)

    async def connect(self):
        if not self.chain:
            raise RuntimeError("DaisyChain configuration not loaded. Call 'load' first!")
        if not self.client:
            self.client = bc.BleClient()
            await self.client.connect()

    async def disconnect(self):
        if self.client:
            await self.client.disconnect()
            self.client = None

    async def upload(self) -> bool:
        if not self.chain or not self.client:
            raise RuntimeError("ConfigTool not properly initialized. Call 'load' and 'connect' first!")

        leds = self.chain.get_leds()
        chunk_size = dc.LED_TOTAL // 12
        leds_list = [leds[i : i + chunk_size] for i in range(0, len(leds), chunk_size)]

        for i, leds in enumerate(leds_list):
            print(f"Uploading chunk {i+1}/{len(leds_list)} with {len(leds)} LEDs ...")
            success = await self._upload_leds(leds)
            if not success:
                print("Failed to upload LED chunk!")
                return False

        print("All LEDs uploaded successfully.")
        self.uploaded = True
        return True

    async def _upload_leds(self, leds: list[dc.Led]) -> bool:
        self.rid_counter += 1
        cmd = cb.CmdBuilder.set_brightness(rid=self.rid_counter, leds=leds)
        response = await self.client.send_command(cmd, timeout=3.0)

        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        print("Response:", doc)
        if "rid" not in doc or doc["rid"] != self.rid_counter or "sts" not in doc or doc["sts"] != 0:
            print("Error:", doc)
            return False
        return True

    async def save(self):
        if not self.chain or not self.client or not self.uploaded:
            raise RuntimeError(
                "ConfigTool not properly initialized or LEDs not uploaded. Call 'load', 'connect' and 'upload' first!"
            )

        self.rid_counter += 1
        cmd = cb.CmdBuilder.save_calibration(rid=self.rid_counter, name=self.chain.name)
        response = await self.client.send_command(cmd, timeout=3.0)

        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        print("Response:", doc)
        if "rid" not in doc or doc["rid"] != self.rid_counter or "sts" not in doc or doc["sts"] != 0:
            print("Error:", doc)
            return False

        print(f"Configuration '{self.chain.name}' successfully saved.")
        return True


async def main():
    config_tool = ConfigTool()
    config_tool.load("chain_config.json")
    await config_tool.connect()

    if await config_tool.upload():
        await config_tool.save()

    await config_tool.disconnect()


if __name__ == "__main__":
    asyncio.run(main())
