import json
import DaisyChain as dc


class CmdBuilder:
    def __init__(self):
        pass

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
    def get_calibration_name(rid: int):
        doc = {
            "rid": rid,
            "cmd": "get_calibration_name",
        }
        json_bytes = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        return json_bytes

    @staticmethod
    def delete_calibration(rid: int):
        doc = {
            "rid": rid,
            "cmd": "delete_calibration",
        }
        json_bytes = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        return json_bytes

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
    def _unpack_sequence(sequence: list[tuple[int, dc.Step]]) -> list[dict]:
        seq = []
        for group_idx, step in sequence:
            seq.append(
                {
                    "group": group_idx,
                    "down": step.down_ms,
                    "pause": step.pause_ms,
                    "up": step.up_ms,
                    "pulse": step.pulse_ms,
                    "return": step.idle_return,
                }
            )
        return seq

    @staticmethod
    def play_show(rid: int, show: dc.Show):
        groups = [CmdBuilder._unpack_leds(leds, index_only=True) for leds in show.get_groups()]
        sequence = CmdBuilder._unpack_sequence(show.get_sequence())
        doc = {
            "rid": rid,
            "cmd": "play_show",
            "groups": groups,
            "sequence": sequence,
        }
        json_bytes = bytearray(json.dumps(doc, separators=(",", ":")) + "\0", "utf-8")
        return json_bytes
