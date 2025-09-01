import pytest_asyncio
import pytest
import asyncio
import json
import re
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
            response = await ble_client.send_command(cmd, timeout=2.0)

    @pytest.mark.asyncio
    async def test_invalid_json(self, ble_client):
        cmd = cb.CmdBuilder.invalid_json()
        assert cmd == bytearray(b'{"rid":0,"cmd":"invalid_json"\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=2.0)

        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        assert "rid" in doc and doc["rid"] == -1
        assert "msg" in doc and doc["msg"] == "Deserialize JSON string failed (IncompleteInput)"
        assert "status" in doc and doc["status"] == -1

    @pytest.mark.asyncio
    async def test_unknown_command(self, ble_client):
        cmd = cb.CmdBuilder.unknown_command()
        assert cmd == bytearray(b'{"rid":0,"cmd":"unknown_command"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=2.0)
        doc = json.loads(response.decode("utf-8").rstrip("\0"))
        assert "rid" in doc and doc["rid"] == 0
        assert "msg" in doc and doc["msg"] == "Unknown 'cmd': 'unknown_command'"
        assert "status" in doc and doc["status"] == -1

    @pytest.mark.asyncio
    async def test_get_version(self, ble_client):
        cmd = cb.CmdBuilder.get_version(rid=1)
        assert cmd == bytearray(b'{"rid":1,"cmd":"get_version"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=2.0)
        version = cb.CmdBuilder.evaluate_get_version_response(response, rid=1)
        assert re.fullmatch(r"V\d+\.\d+\.\d+", version)

    @pytest.mark.asyncio
    async def test_get_system_id(self, ble_client):
        cmd = cb.CmdBuilder.get_system_id(rid=11)
        assert cmd == bytearray(b'{"rid":11,"cmd":"get_system_id"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=2.0)
        system_id = cb.CmdBuilder.evaluate_get_system_id_response(response, rid=11)
        assert isinstance(system_id, str) and len(system_id) > 0

    @pytest.mark.asyncio
    async def test_set_system_id(self, ble_client):
        cmd = cb.CmdBuilder.set_system_id(rid=12, system_id="Who is John Galt?")
        assert cmd == bytearray(b'{"rid":12,"cmd":"set_system_id","system_id":"Who is John Galt?"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=2.0)
        assert cb.CmdBuilder.evaluate_set_system_id_response(response, rid=12) == True

    @pytest.mark.asyncio
    async def test_get_calibration_name(self, ble_client):
        cmd = cb.CmdBuilder.get_calibration_name(rid=2)
        assert cmd == bytearray(b'{"rid":2,"cmd":"get_calibration_name"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=2.0)
        name = cb.CmdBuilder.evaluate_get_calibration_name_response(response, rid=2)
        assert isinstance(name, str) and len(name) > 0

    @pytest.mark.asyncio
    async def test_delete_calibration(self, ble_client):
        cmd = cb.CmdBuilder.delete_calibration(rid=3)
        assert cmd == bytearray(b'{"rid":3,"cmd":"delete_calibration"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=3.0)
        assert cb.CmdBuilder.evaluate_delete_calibration_response(response, rid=3) == True

    @pytest.mark.asyncio
    async def test_save_calibration(self, ble_client):
        cmd = cb.CmdBuilder.save_calibration(rid=4, name="test_42_name_69")
        assert cmd == bytearray(b'{"rid":4,"cmd":"save_calibration","name":"test_42_name_69"}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=3.0)
        assert cb.CmdBuilder.evaluate_save_calibration_response(response, rid=4) == True

    @pytest.mark.asyncio
    async def test_set_brightness(self, ble_client):
        leds = [dc.Led(pcb_index=1, led_index=2, brightness=42), dc.Led(pcb_index=2, led_index=3, brightness=69)]
        cmd = cb.CmdBuilder.set_brightness(rid=5, leds=leds)
        assert cmd == bytearray(b'{"rid":5,"cmd":"set_brightness","leds":[[1,2,42],[2,3,69]]}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=3.0)
        assert cb.CmdBuilder.evaluate_set_brightness_response(response, rid=5) == True

    @pytest.mark.asyncio
    async def test_get_brightness(self, ble_client):
        leds = [dc.Led(pcb_index=1, led_index=2, brightness=0), dc.Led(pcb_index=2, led_index=3, brightness=0)]
        cmd = cb.CmdBuilder.get_brightness(rid=6, leds=leds)
        assert cmd == bytearray(b'{"rid":6,"cmd":"get_brightness","leds":[[1,2],[2,3]]}\0')

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=3.0)
        downloaded_leds = cb.CmdBuilder.evaluate_get_brightness_response(response, rid=6)
        assert isinstance(downloaded_leds, list) and len(downloaded_leds) == 2
        assert all(isinstance(led, dc.Led) for led in downloaded_leds)

    @pytest.mark.asyncio
    async def test_play_show(self, ble_client):
        groups = [
            [dc.Led(pcb_index=1, led_index=1, brightness=0), dc.Led(pcb_index=1, led_index=2, brightness=0)],
            [dc.Led(pcb_index=2, led_index=3, brightness=0), dc.Led(pcb_index=2, led_index=4, brightness=0)],
        ]
        sequence = [
            (0, dc.Step(down_ms=500, pause_ms=500, up_ms=500, pulse_ms=500, reps=10, idle_return=False)),
            (1, dc.Step(down_ms=0, pause_ms=500, up_ms=0, pulse_ms=500, reps=10, idle_return=True)),
        ]
        show = dc.Show(name="test_show")
        for group in groups:
            show.add_group(group)
        for group_idx, step in sequence:
            show.add_step(group_idx, step)

        cmd = cb.CmdBuilder.play_show(rid=7, show=show, force=True)
        expected_cmd = (
            b'{"rid":7,"cmd":"play_show","force":1,"groups":[[[1,1],[1,2]],[[2,3],[2,4]]],'
            b'"sequence":[[0,500,500,500,500,10,0],[1,0,500,0,500,10,1]]}\0'
        )
        assert cmd == bytearray(expected_cmd)

        # Send command and evaluate response
        response = await ble_client.send_command(cmd, timeout=5.0)
        assert cb.CmdBuilder.evaluate_play_show_response(response, rid=7) == True
