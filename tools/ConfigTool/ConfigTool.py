import json
import asyncio
import CmdBuilder as cb
import DaisyChain as dc
import BleClient as bc


async def main():
    chain = dc.DaisyChain()
    chain.load_config("chain_config.json")

    client = bc.BleClient()
    await client.connect()

    leds = chain.get_leds()
    cmd = cb.CmdBuilder.set_brightness(rid=1, leds=leds)

    response = await client.send_command(cmd, timeout=5.0)
    doc = json.loads(response.decode("utf-8").rstrip("\0"))
    print("Response:", doc)

    if doc["sts"] == 0:
        print("Brightness set successfully.")
        cmd = cb.CmdBuilder.save_calibration(rid=2, name=chain.name)
        response = await client.send_command(cmd, timeout=5.0)
        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        print("Save Response:", doc)

    await client.disconnect()


if __name__ == "__main__":
    asyncio.run(main())
