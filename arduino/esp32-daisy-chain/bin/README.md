# Upload firmware to ESP32-S3
The firmware can be uploaded via BLE-OTA or Serial Port. The first time
the firmware must be uploaded using the esptool and Serial Port.


## Upload with web app via BLE-OTA
1. Go to https://gb88.github.io/BLEOTA/
2. Connect to device via BLE
3. Select the new firmware bin file and upload it


## Upload with esptool via Serial Port (no flash erase)
1. Install esptool
    ```bash
    pip install esptool
    ```

2. Start bootloader (the order of the following instructions is crucial!)
    1. Press the RESET and BOOT button
    2. Release RESET button
    3. Release BOOT button

3. Upload application bin
    ```bash
    python -m esptool --chip esp32s3 --port COM3 --baud 460800 write-flash -z 0x10000 daisy-chain_V0.0.2_release.bin
    ```

## Upload with esptool via Serial Port (with flash erase)
This is not needed and all configuration data will be lost and needs to be rewritten.
If you don't know what you are doing follow the previous upload section.

1. Start bootloader ...

2. Erase flash (skip this if you don't know what you are doing)
    ```bash
    python -m esptool --chip esp32s3 --port COM3 erase-flash
    ```

3. Upload bins (bootloader, partitions and firmware)  
    Note: Upload all 3 bins in one command as stated below
    ```bash
    python -m esptool --chip esp32s3 --port COM3 --baud 460800 write-flash -z 0x0 daisy-chain.bootloader.bin 0x8000 daisy-chain.partitions.bin 0x10000 daisy-chain_V0.0.2_release.bin
    ```

4. Start ConfigToolApp.exe
    1. Upload new configuration
    2. Save configuration
