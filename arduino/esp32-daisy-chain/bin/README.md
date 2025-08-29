# Upload firmware to ESP32-S3

1. Install esptool
    ```bash
    pip install esptool
    ```

2. Start bootloader (the order of the following instructions is crucial!)
    1. Press the RESET and BOOT button
    2. Release RESET button
    3. Release BOOT button

2. Erase flash
    ```bash
    python -m esptool --chip esp32s3 --port COM3 erase-flash
    ```

3. Upload bins (bootloader, partitions and firmware)  
    Note: Upload all 3 bins in one command as stated below
    ```bash
        python -m esptool --chip esp32s3 --port COM3 --baud 460800 write-flash -z 0x0 daisy-chain.ino.bootloader.bin 0x8000 daisy-chain.ino.partitions.bin 0x10000 daisy-chain.ino_V0.0.2_release.bin
    ```
    Note: Upload all 3 bins in one command as stated below

4. Start ConfigToolApp.exe
    1. Upload new configuration
    2. Save configuration
