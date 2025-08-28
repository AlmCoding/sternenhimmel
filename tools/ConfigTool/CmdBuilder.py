import json
import DaisyChain as dc


class CmdBuilder:
    def __init__(self):
        pass

    @staticmethod
    def _evaluate_response(response: bytearray, **kwargs) -> tuple[bool, object | None]:
        try:
            doc = json.loads(response.decode("utf-8").rstrip("\0"))
        except json.JSONDecodeError:
            return (False, None)

        last_key = None
        for key, value in kwargs.items():
            last_key = key
            if key not in doc:
                return (False, None)

            if isinstance(value, type):
                if not isinstance(doc[key], value):
                    return (False, None)
            else:
                if doc[key] != value:
                    return (False, None)

        return (True, doc[last_key] if last_key else None)

    @staticmethod
    def invalid_frame():
        # Missing null terminator => causes timeout on ESP32 side
        return bytearray(b'{"rid":0,"cmd":"invalid_frame"}')

    @staticmethod
    def invalid_json():
        # Missing closing brace => causes JSON parse error on ESP32 side
        return bytearray(b'{"rid":0,"cmd":"invalid_json"\0')

    @staticmethod
    def unknown_command():
        # Unknown command => causes "unknown command" error on ESP32 side
        return bytearray(b'{"rid":0,"cmd":"unknown_command"}\0')

    @staticmethod
    def get_version(rid: int):
        doc = {
            "rid": rid,
            "cmd": "get_version",
        }
        json_bytes = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        return json_bytes

    @staticmethod
    def evaluate_get_version_response(response: bytearray, rid: int) -> str | None:
        _, version = CmdBuilder._evaluate_response(response, rid=rid, status=0, version=str)
        return version

    @staticmethod
    def get_calibration_name(rid: int):
        doc = {
            "rid": rid,
            "cmd": "get_calibration_name",
        }
        json_bytes = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        return json_bytes

    @staticmethod
    def evaluate_get_calibration_name_response(response: bytearray, rid: int) -> str | None:
        _, name = CmdBuilder._evaluate_response(response, rid=rid, status=0, name=str)
        return name

    @staticmethod
    def delete_calibration(rid: int):
        doc = {
            "rid": rid,
            "cmd": "delete_calibration",
        }
        json_bytes = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        return json_bytes

    @staticmethod
    def evaluate_delete_calibration_response(response: bytearray, rid: int) -> bool:
        success, _ = CmdBuilder._evaluate_response(response, rid=rid, status=0)
        return success

    @staticmethod
    def save_calibration(rid: int, name: str):
        doc = {
            "rid": rid,
            "cmd": "save_calibration",
            "name": name,
        }
        json_bytes = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        return json_bytes

    @staticmethod
    def evaluate_save_calibration_response(response: bytearray, rid: int) -> bool:
        success, _ = CmdBuilder._evaluate_response(response, rid=rid, status=0)
        return success

    @staticmethod
    def _unpack_leds(leds: list[dc.Led], index_only: bool) -> list[list[int]]:
        if len(leds) == 0 or len(leds) > dc.LED_TOTAL:
            raise ValueError(f"leds length {len(leds)} out of range [1, {dc.LED_TOTAL}]")
        if index_only:
            # leds: [[pcb_index, led_index], ...]
            return [[led.pcb_index, led.led_index] for led in leds]
        else:
            # leds: [[pcb_index, led_index, brightness], ...]
            return [[led.pcb_index, led.led_index, led.brightness] for led in leds]

    @staticmethod
    def set_brightness(rid: int, leds: list[dc.Led]):
        leds = CmdBuilder._unpack_leds(leds, index_only=False)
        doc = {
            "rid": rid,
            "cmd": "set_brightness",
            "leds": leds,
        }
        json_bytes = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        return json_bytes

    @staticmethod
    def evaluate_set_brightness_response(response: bytearray, rid: int) -> bool:
        success, _ = CmdBuilder._evaluate_response(response, rid=rid, status=0)
        return success

    @staticmethod
    def get_brightness(rid: int, leds: list[dc.Led]):
        leds = CmdBuilder._unpack_leds(leds, index_only=True)
        doc = {
            "rid": rid,
            "cmd": "get_brightness",
            "leds": leds,
        }
        json_bytes = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        return json_bytes

    @staticmethod
    def evaluate_get_brightness_response(response: bytearray, rid: int) -> list[dc.Led] | None:
        success, doc_leds = CmdBuilder._evaluate_response(response, rid=rid, status=0, leds=list)
        if not success or not isinstance(doc_leds, list):
            return None

        leds = []
        for item in doc_leds:
            if not isinstance(item, list) or len(item) != 3 or not all(isinstance(x, int) for x in item):
                return None
            leds.append(dc.Led(pcb_index=item[0], led_index=item[1], brightness=item[2]))
        return leds

    @staticmethod
    def _unpack_sequence(sequence: list[tuple[int, dc.Step]]) -> list[dict]:
        seq = []
        for group_idx, step in sequence:
            seq.append(
                [group_idx, step.down_ms, step.pause_ms, step.up_ms, step.pulse_ms, step.reps, int(step.idle_return)]
            )
        return seq

    @staticmethod
    def play_show(rid: int, show: dc.Show, force: bool = False):
        groups = [CmdBuilder._unpack_leds(leds, index_only=True) for leds in show.get_groups()]
        sequence = CmdBuilder._unpack_sequence(show.get_sequence())
        doc = {
            "rid": rid,
            "cmd": "play_show",
            "force": int(force),
            "groups": groups,
            "sequence": sequence,
        }
        json_bytes = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        return json_bytes

    @staticmethod
    def evaluate_play_show_response(response: bytearray, rid: int) -> bool:
        success, _ = CmdBuilder._evaluate_response(response, rid=rid, status=0)
        return success

    @staticmethod
    def stop_show(rid: int):
        doc = {
            "rid": rid,
            "cmd": "stop_show",
        }
        json_bytes = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        return json_bytes

    @staticmethod
    def evaluate_stop_show_response(response: bytearray, rid: int) -> bool:
        success, _ = CmdBuilder._evaluate_response(response, rid=rid, status=0)
        return success
