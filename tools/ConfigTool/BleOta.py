# Copyright 2024 Ryan Powell and NimBLEOta contributors
# Sponsored by Theengs https://www.theengs.io, https://github.com/theengs
# MIT License

import asyncio
import os
from bleak import uuids, BleakClient
from helper import format_log_message


OTA_SERVICE_UUID = uuids.normalize_uuid_16(0x8018)
OTA_COMMAND_UUID = uuids.normalize_uuid_16(0x8022)
OTA_FIRMWARE_UUID = uuids.normalize_uuid_16(0x8020)
START_COMMAND = 0x0001
STOP_COMMAND = 0x0002
ACK_COMMAND = 0x0003
ACK_ACCEPTED = 0x0000
ACK_REJECTED = 0x0001
FW_ACK_SUCCESS = 0x0000
FW_ACK_CRC_ERROR = 0x0001
FW_ACK_SECTOR_ERROR = 0x0002
FW_ACK_LEN_ERROR = 0x0003
RSP_CRC_ERROR = 0xFFFF


class BleOta:
    def __init__(self):
        self.print_cb = None

    def log(self, message):
        message = format_log_message(message, "[BleOta]")
        if self.print_cb:
            self.print_cb(message)
        else:
            print(message)

    def register_print_callback(self, print_cb):
        self.print_cb = print_cb

    def crc16_ccitt(self, buf):
        crc16 = 0
        for byte in buf:
            crc16 ^= byte << 8
            for _ in range(8):
                if crc16 & 0x8000:
                    crc16 = (crc16 << 1) ^ 0x1021
                else:
                    crc16 = crc16 << 1
                crc16 &= 0xFFFF  # Ensure crc16 remains a 16-bit value
        return crc16

    async def fw_notification_handler(self, sender, data, queue):
        if len(data) == 20:
            sector_sent = int.from_bytes(data[0:2], byteorder="little")
            status = int.from_bytes(data[2:4], byteorder="little")
            cur_sector = int.from_bytes(data[4:6], byteorder="little")
            crc = int.from_bytes(data[18:20], byteorder="little")

            if self.crc16_ccitt(data[0:18]) != crc:
                status = RSP_CRC_ERROR

            await queue.put((status, cur_sector))

    async def cmd_notification_handler(self, sender, data, queue):
        if len(data) == 20:
            ack = int.from_bytes(data[0:2], byteorder="little")
            cmd = int.from_bytes(data[2:4], byteorder="little")
            rsp = int.from_bytes(data[4:6], byteorder="little")
            crc = int.from_bytes(data[18:20], byteorder="little")

            if self.crc16_ccitt(data[0:18]) != crc:
                self.log("Command response CRC error")
                rsp = RSP_CRC_ERROR

            await queue.put(rsp)

    async def upload_sector(self, client, sector, sec_idx):
        max_bytes = min(512, client.mtu_size - 3) - 3  # 3 bytes for the packet header, 3 bytes for the BLE overhead
        chunks = [sector[i : i + max_bytes] for i in range(0, len(sector), max_bytes)]
        for sequence, chunk in enumerate(chunks):
            if sequence == len(chunks) - 1:
                sequence = 0xFF  # Indicate to peer this is the last chunk of sector

            data = sec_idx.to_bytes(2, byteorder="little")
            data += sequence.to_bytes(1, byteorder="little")
            data += chunk
            await client.write_gatt_char(OTA_FIRMWARE_UUID, data, response=False)

    async def update_device(self, client: BleakClient, file_size: int, sectors: list):
        success = False
        queue = asyncio.Queue()
        await client.start_notify(
            OTA_COMMAND_UUID,
            lambda sender, data: asyncio.create_task(self.cmd_notification_handler(sender, data, queue)),
        )
        self.log("Sending start command")
        command = bytearray(20)
        command[0:2] = START_COMMAND.to_bytes(2, byteorder="little")
        command[2:6] = file_size.to_bytes(4, byteorder="little")
        crc16 = self.crc16_ccitt(command[0:18])
        command[18:20] = crc16.to_bytes(2, byteorder="little")

        while True:
            await client.write_gatt_char(OTA_COMMAND_UUID, command)
            ack = await queue.get()
            if ack != RSP_CRC_ERROR:
                break

        if ack == ACK_ACCEPTED:
            await client.start_notify(
                OTA_FIRMWARE_UUID,
                lambda sender, data: asyncio.create_task(self.fw_notification_handler(sender, data, queue)),
            )
            self.log("Sending firmware...")
            sec_idx = 0
            sec_count = len(sectors)
            while sec_idx < sec_count:
                sector = sectors[sec_idx]
                self.log(f"Sending sector {sec_idx}/{sec_count-1}, progress: {round(sec_idx/(sec_count-1)*100,1)} %")
                await self.upload_sector(
                    client, sector, sec_idx if len(sector) == 4098 else 0xFFFF
                )  # send last sector as 0xFFFF
                ack, rsp_sector = await queue.get()

                if ack == FW_ACK_SUCCESS:
                    if sec_idx == sec_count - 1:
                        self.log("OTA update complete")
                        success = True
                        break
                    sec_idx += 1

                elif ack == FW_ACK_CRC_ERROR or ack == FW_ACK_LEN_ERROR or ack == RSP_CRC_ERROR:
                    self.log("Length Error" if ack == FW_ACK_LEN_ERROR else "CRC Error", "- Retrying sector")

                elif ack == FW_ACK_SECTOR_ERROR:
                    self.log(f"Sector Error, sending sector: {rsp_sector}")
                    sec_idx = rsp_sector

                else:
                    self.log("Unknown error!")
                    break
        else:
            self.log("Start command rejected!")
        return success

    async def perform_ota_update(self, client: BleakClient, file_name: str):
        if not os.path.isfile(file_name):
            self.log(f"Invalid file: '{file_name}'")
            return False

        file_size = os.path.getsize(file_name) & 0xFFFFFFFF
        if not file_size:
            self.log(f"Invalid file size: {file_size}")
            return False

        sectors = []
        with open(file_name, "rb") as file:
            while True:
                sector = file.read(4096)
                if not sector:
                    break
                sector += self.crc16_ccitt(sector).to_bytes(2, byteorder="little")
                sectors.append(sector)

        return await self.update_device(client, file_size, sectors)
