import pytest_asyncio
import pytest
import asyncio
import json
import CmdBuilder as cb
import DaisyChain as dc
import BleClient as bc


@pytest_asyncio.fixture
async def ble_client():
    client = bc.BleClient()
    await asyncio.sleep(1)
    await client.connect()
    yield client
    await client.disconnect()


class TestCmdBuilder:
    @pytest.mark.asyncio
    async def test_invalid_frame(self, ble_client):
        cmd = cb.CmdBuilder.invalid_frame()
        assert cmd == bytearray(b'{"rid":0,"cmd":"invalid_frame"}')

        # Send command and evaluate response
        with pytest.raises(asyncio.TimeoutError):
            response = await ble_client.send_command(cmd, timeout=1.0)

    @pytest.mark.asyncio
    async def test_invalid_json(self, ble_client):
        cmd = cb.CmdBuilder.invalid_json()
        assert cmd == bytearray(b'{"rid":0,"cmd":"invalid_json"\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=1.0)
        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        assert "rid" in doc and doc["rid"] == -1
        assert "msg" in doc and doc["msg"] == "Deserialize JSON string failed (IncompleteInput)"
        assert "sts" in doc and doc["sts"] == -1

    @pytest.mark.asyncio
    async def test_unknown_command(self, ble_client):
        cmd = cb.CmdBuilder.unknown_command()
        assert cmd == bytearray(b'{"rid":0,"cmd":"unknown_command"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=1.0)
        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        assert "rid" in doc and doc["rid"] == 0
        assert "msg" in doc and doc["msg"] == "Unknown 'cmd': 'unknown_command'"
        assert "sts" in doc and doc["sts"] == -1

    @pytest.mark.asyncio
    async def test_get_version(self, ble_client):
        cmd = cb.CmdBuilder.get_version(rid=42)
        assert cmd == bytearray(b'{"rid":42,"cmd":"get_version"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=1.0)
        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        assert "rid" in doc and doc["rid"] == 42
        assert "msg" in doc and doc["msg"] == "v0.0.1"
        assert "sts" in doc and doc["sts"] == 0

    @pytest.mark.asyncio
    async def test_get_calibration_name(self, ble_client):
        cmd = cb.CmdBuilder.get_calibration_name(rid=8)
        assert cmd == bytearray(b'{"rid":8,"cmd":"get_calibration_name"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=1.0)
        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        assert "rid" in doc and doc["rid"] == 8
        assert "msg" in doc and isinstance(doc["msg"], str)
        assert "sts" in doc and doc["sts"] == 0

    @pytest.mark.asyncio
    async def test_delete_calibration(self, ble_client):
        cmd = cb.CmdBuilder.delete_calibration(rid=7)
        assert cmd == bytearray(b'{"rid":7,"cmd":"delete_calibration"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=1.0)
        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        assert "rid" in doc and doc["rid"] == 7
        assert "sts" in doc and doc["sts"] == 0

    @pytest.mark.asyncio
    async def test_save_calibration(self, ble_client):
        cmd = cb.CmdBuilder.save_calibration(rid=3, name="test_name_42")
        assert cmd == bytearray(b'{"rid":3,"cmd":"save_calibration","name":"test_name_42"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=1.0)
        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        assert "rid" in doc and doc["rid"] == 3
        assert "sts" in doc and doc["sts"] == 0

    @pytest.mark.asyncio
    async def test_set_brightness(self, ble_client):
        leds = [dc.Led(pcb_index=1, led_index=2, brightness=42), dc.Led(pcb_index=2, led_index=3, brightness=69)]
        cmd = cb.CmdBuilder.set_brightness(rid=1, leds=leds)
        assert cmd == bytearray(b'{"rid":1,"cmd":"set_brightness","leds":[[1,2,42],[2,3,69]]}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=1.0)
        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        assert "rid" in doc and doc["rid"] == 1
        assert "sts" in doc and doc["sts"] == 0

    @pytest.mark.asyncio
    async def test_get_brightness(self, ble_client):
        leds = [dc.Led(pcb_index=1, led_index=2, brightness=0), dc.Led(pcb_index=2, led_index=3, brightness=0)]
        cmd = cb.CmdBuilder.get_brightness(rid=5, leds=leds)
        assert cmd == bytearray(b'{"rid":5,"cmd":"get_brightness","leds":[[1,2],[2,3]]}\0')

        response = await ble_client.send_command(cmd, timeout=1.0)
        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        assert "rid" in doc and doc["rid"] == 5
        assert "sts" in doc and doc["sts"] == 0
        assert "leds" in doc and isinstance(doc["leds"], list)
        assert len(doc["leds"]) == 2
