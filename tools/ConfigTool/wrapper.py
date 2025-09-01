import datetime
import sys
import io
from ConfigTool import ConfigTool


CONFIG_TOOL = ConfigTool()
PRINT_CB = None


def register_print_callback(print_cb):
    global CONFIG_TOOL, PRINT_CB
    PRINT_CB = print_cb
    CONFIG_TOOL.register_print_callback(print_cb)


async def async_connect():
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        status = await CONFIG_TOOL.connect()
    except Exception as e:
        PRINT_CB(f"Error connecting to device: {e}")
    return status


async def async_disconnect():
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        status = await CONFIG_TOOL.disconnect()
    except Exception as e:
        PRINT_CB(f"Error disconnecting from device: {e}")
    return status


async def async_set_system_id(system_id):
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        status = await CONFIG_TOOL.set_system_id(system_id)
    except Exception as e:
        PRINT_CB(f"Error setting system ID: {e}")
    return status


async def async_get_info():
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        info = await CONFIG_TOOL.get_info()
        status = info is not None
    except Exception as e:
        PRINT_CB(f"Error getting device info: {e}")
    return status, info


async def async_load_config(file_path):
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        status = await CONFIG_TOOL.load_config(file_path)
    except Exception as e:
        PRINT_CB(f"Error loading config: {e}")
    return status


async def async_upload_config():
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        status = await CONFIG_TOOL.upload_config()
    except Exception as e:
        PRINT_CB(f"Error uploading config: {e}")
    return status


async def async_verify_config():
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        status = await CONFIG_TOOL.verify_config()
    except Exception as e:
        PRINT_CB(f"Error verifying config: {e}")
    return status


async def async_save_config():
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        status = await CONFIG_TOOL.save_config()
    except Exception as e:
        PRINT_CB(f"Error saving config: {e}")
    return status


async def async_load_cmd(file_path):
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        status = await CONFIG_TOOL.load_cmd(file_path)
    except Exception as e:
        PRINT_CB(f"Error loading command: {e}")
    return status


async def async_send_cmd():
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        status = await CONFIG_TOOL.send_cmd()
    except Exception as e:
        PRINT_CB(f"Error sending command: {e}")
    return status


async def async_stop_show():
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        status = await CONFIG_TOOL.stop_show()
    except Exception as e:
        PRINT_CB(f"Error stopping sequence: {e}")
    return status


async def async_ota_update(file_path):
    global CONFIG_TOOL, PRINT_CB
    assert PRINT_CB is not None, "Print callback not registered"
    status = False
    try:
        status = await CONFIG_TOOL.upload_ota(file_path)
    except Exception as e:
        PRINT_CB(f"Error during OTA update: {e}")
    return status
