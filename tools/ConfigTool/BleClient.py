import asyncio
from bleak import BleakScanner
from bleak import BleakClient


# Name of ESP32 BLE device
SERVER_DEVICE_NAME = "Sternenhimmel"

# UUIDs of ESP32 UART service
SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
CHARACTERISTIC_UUID_RX = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
CHARACTERISTIC_UUID_TX = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

ATT_OVERHEAD = 3  # ATT header size for notifications/indications
NET_MTU = 256 - ATT_OVERHEAD  # Effective MTU for data transfer


class BleClient:
    def __init__(self):
        self.client: BleakClient | None = None
        self.response_buffer = bytearray()

    @staticmethod
    async def scan_for_device(device_name):
        print("Scanning for BLE devices ...")
        devices = await BleakScanner.discover()
        for device in devices:
            print(device)
            if device.name == device_name:
                return device
        return None

    async def connect(self, device_name=SERVER_DEVICE_NAME) -> bool:
        target = await BleClient.scan_for_device(device_name)
        if not target:
            raise RuntimeError(f"BLE device '{device_name}' not found!")

        print(f"Connecting to {target.name} [{target.address}] ...")
        self.client = BleakClient(target.address)
        await self.client.connect()
        print("Connected:", self.client.is_connected)

        if self.client.is_connected:
            self.response_buffer.clear()

            def handle_indication(sender, data: bytearray):
                # This gets called when an indication arrives.
                # The OS automatically sends the confirmation back to the ESP32.
                print(f"Indication from {sender}, data: {data}")
                self.response_buffer += data

            # Subscribe to TX indications (same API as notifications)
            await self.client.start_notify(CHARACTERISTIC_UUID_TX, handle_indication)
            await asyncio.sleep(0.5)  # wait for subscription to be set up

        return self.client.is_connected

    async def disconnect(self) -> bool:
        if self.client and self.client.is_connected:
            await self.client.stop_notify(CHARACTERISTIC_UUID_TX)
            await self.client.disconnect()
            self.client = None
        print("Disconnected!")
        return True

    async def send_command(self, command: bytearray, timeout=1.0) -> bytearray:
        if not self.client or not self.client.is_connected:
            raise RuntimeError("Not connected to BLE device!")

        self.response_buffer.clear()
        print(f"Sending: {command}")

        data_list = [command[i : i + NET_MTU] for i in range(0, len(command), NET_MTU)]
        for data in data_list:
            await self.client.write_gatt_char(CHARACTERISTIC_UUID_RX, data, response=True)

        # Wait for response
        print("Waiting for response (timeout = %.1f s) ..." % timeout)
        while not b"\0" in self.response_buffer:
            await asyncio.sleep(0.1)
            timeout -= 0.1
            if timeout <= 0:
                raise TimeoutError("Timeout waiting for response from BLE device!")

        return self.response_buffer
