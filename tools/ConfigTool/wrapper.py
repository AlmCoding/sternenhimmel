import datetime
import sys
import io
from ConfigTool import ConfigTool


CONFIG_TOOL = ConfigTool()


class TimestampedIO(io.StringIO):
    def write(self, s):
        # Split into lines, only add prefix to non-empty ones
        for line in s.splitlines(True):  # keep line endings
            if line.strip() != "":
                ts = datetime.datetime.now().strftime("[%H:%M:%S] ")
                super().write(ts + line)
            else:
                super().write(line)


class PrintBuffer:
    def __enter__(self):
        self._buffer = TimestampedIO()
        self._old_stdout = sys.stdout
        sys.stdout = self._buffer
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        sys.stdout = self._old_stdout

    def get_value(self):
        return self._buffer.getvalue()


async def async_connect():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.connect()
        except Exception as e:
            print(f"Error connecting to device: {e}")
    return status, buf.get_value()


async def async_disconnect():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.disconnect()
        except Exception as e:
            print(f"Error disconnecting from device: {e}")
    return status, buf.get_value()


async def async_get_info():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            info = await CONFIG_TOOL.get_info()
            status = info is not None
        except Exception as e:
            print(f"Error getting device info: {e}")
    return status, buf.get_value()


async def async_load_config(file_path):
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.load_config(file_path)
        except Exception as e:
            print(f"Error loading config: {e}")
    return status, buf.get_value()


async def async_upload_config():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.upload_config()
        except Exception as e:
            print(f"Error uploading config: {e}")
    return status, buf.get_value()


async def async_verify_config():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.verify_config()
        except Exception as e:
            print(f"Error verifying config: {e}")
    return status, buf.get_value()


async def async_save_config():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.save_config()
        except Exception as e:
            print(f"Error saving config: {e}")
    return status, buf.get_value()


async def async_load_cmd(file_path):
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.load_cmd(file_path)
        except Exception as e:
            print(f"Error loading command: {e}")
    return status, buf.get_value()


async def async_send_cmd():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.send_cmd()
        except Exception as e:
            print(f"Error sending command: {e}")
    return status, buf.get_value()


async def async_stop_show():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.stop_show()
        except Exception as e:
            print(f"Error stopping sequence: {e}")
    return status, buf.get_value()


async def async_ota_update(file_path):
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.upload_ota(file_path)
        except Exception as e:
            print(f"Error during OTA update: {e}")
    return status, buf.get_value()
