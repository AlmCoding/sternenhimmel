import asyncio
from bleak import BleakScanner
from bleak import BleakClient

# UUIDs of your ESP32 UART service
SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
CHARACTERISTIC_UUID_RX = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
CHARACTERISTIC_UUID_TX = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


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
            # Send test data to RX
            message = "ABC\0"
            print(f"Sending: {message.strip()}")
            await client.write_gatt_char(CHARACTERISTIC_UUID_RX, message.encode())

            # Listen for TX notifications (optional)
            def handle_notification(sender, data: bytearray):
                print(f"Notification from {sender}: {data.decode(errors='ignore')}")

            await client.start_notify(CHARACTERISTIC_UUID_TX, handle_notification)

            # Keep connection alive to receive notifications
            print("Waiting for responses (10s)...")
            await asyncio.sleep(10)

            await client.stop_notify(CHARACTERISTIC_UUID_TX)

asyncio.run(main())
