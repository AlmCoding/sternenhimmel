import asyncio
from bleak import BleakScanner
from bleak import BleakClient
import time

# UUIDs of your ESP32 UART service
SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
CHARACTERISTIC_UUID_RX = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
CHARACTERISTIC_UUID_TX = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

def CMD_GET_VERSION(rid: int):
    return '{"rid":%d,"cmd":"get_version"}\0' % rid  # Get version
def CMD_DELETE_CALIBRATION(rid: int):
    return '{"rid":%d,"cmd":"delete_calibration"}\0' % rid  # Restores default values
def CMD_SAVE_CALIBRATION(rid: int, name: str):
   return '{"rid":%d,"cmd":"save_calibration","name":"%s"}\0' % (rid, name)  # Saves current idle brightnesses


async def scan_for_device(device_name):
    print("Scanning for BLE devices...")
    devices = await BleakScanner.discover()

    for device in devices:
        print(device)
        if device.name == device_name:
            return device

    print("Target device not found!")
    return None


async def main():
    device_name = "Sternenhimmel"
    target = await scan_for_device(device_name)
    if not target:
        print(f"Device '{device_name}' not found!")
        return

    print(f"Connecting to {target.name} [{target.address}]...")
    async with BleakClient(target.address) as client:
        print("Connected:", client.is_connected)

        if client.is_connected:
            response_buffer = bytearray()

            def handle_indication(sender, data: bytearray):
                # This gets called when an indication arrives.
                # The OS automatically sends the confirmation back to the ESP32.
                print(f"Indication from {sender}, data: {data}")
                nonlocal response_buffer
                response_buffer += data

            # Subscribe to TX indications (same API as notifications)
            await client.start_notify(CHARACTERISTIC_UUID_TX, handle_indication)
            time.sleep(1)

            # Send command to RX (write with response, reliable)
            message = CMD_GET_VERSION(0)
            print(f"Sending: {message.strip()}")
            await client.write_gatt_char(CHARACTERISTIC_UUID_RX, message.encode())  # WRITE (with response)

            # Wait for the whole transfer (depends on your protocol!)
            await asyncio.sleep(10)

            await client.stop_notify(CHARACTERISTIC_UUID_TX)


asyncio.run(main())
