import json

PCB_COUNT = 60
LED_COUNT = 12
LED_TOTAL = PCB_COUNT * LED_COUNT
MAX_BRIGHTNESS = 100


class Led:
    def __init__(self, pcb_index: int, led_index: int, brightness: int):
        if not (0 < pcb_index <= PCB_COUNT):
            raise ValueError(f"pcb_index ({pcb_index}) out of range [1, {PCB_COUNT}]")
        if not (0 < led_index <= LED_COUNT):
            raise ValueError(f"led_index ({led_index}) out of range [1, {LED_COUNT}]")
        if not (0 <= brightness <= MAX_BRIGHTNESS):
            raise ValueError(f"brightness ({brightness}) out of range [0, {MAX_BRIGHTNESS}]")
        self.pcb_index = pcb_index
        self.led_index = led_index
        self.brightness = brightness


class Step:
    def __init__(self, down_ms: int, pause_ms: int, up_ms: int, pulse_ms: int, reps: int, idle_return: bool = False):
        if down_ms < 0:
            raise ValueError(f"down_ms ({down_ms}) must be non-negative!")
        if pause_ms < 0:
            raise ValueError(f"pause_ms ({pause_ms}) must be non-negative!")
        if up_ms < 0:
            raise ValueError(f"up_ms ({up_ms}) must be non-negative!")
        if pulse_ms < 0:
            raise ValueError(f"pulse_ms ({pulse_ms}) must be non-negative!")
        if reps < 1:
            raise ValueError(f"reps ({reps}) must be larger than zero!")
        self.down_ms = down_ms
        self.pause_ms = pause_ms
        self.up_ms = up_ms
        self.pulse_ms = pulse_ms
        self.reps = reps
        self.idle_return = idle_return


class Show:
    def __init__(self, name: str):
        self.name = name
        self.groups = []
        self.sequence = []

    def add_group(self, leds: list[Led]):
        if len(leds) == 0 or len(leds) > LED_TOTAL:
            raise ValueError(f"leds length {len(leds)} out of range [1, {LED_TOTAL}]")
        self.groups.append(leds)

    def add_step(self, group_idx: int, step: Step):
        if not (0 <= group_idx < len(self.groups)):
            raise ValueError(f"group_idx {group_idx} out of range [0, {len(self.groups)-1}]")
        self.sequence.append((group_idx, step))

    def get_groups(self) -> list[list[Led]]:
        return self.groups

    def get_sequence(self) -> list[tuple[int, Step]]:
        return self.sequence


class DaisyChain:
    def __init__(self):
        self.file_path = ""
        self.name = ""
        self.groups = {}
        self.leds = []
        self.ConfigLedKeys = ("pcb_idx", "led_idx", "group", "correction")

    def load_config(self, file_path: str):
        if file_path != self.file_path:
            print(f"Loading config file: '{file_path}'")
            self.file_path = file_path
            self.leds = []  # Clear existing LEDs if loading a different file
        else:
            print(f"Reloading config file: '{file_path}'")

        with open(file_path, "r", encoding="utf-8") as f:
            doc = json.load(f)

        if "name" not in doc or not isinstance(doc["name"], str):
            raise ValueError("Invalid config file: 'name' key missing or not a string")
        if "groups" not in doc or not isinstance(doc["groups"], dict):
            raise ValueError("Invalid config file: 'groups' key missing or not a dict")
        if "leds" not in doc or not isinstance(doc["leds"], list):
            raise ValueError("Invalid config file: 'leds' key missing or not a list")

        self.name = doc["name"]
        self.groups = doc["groups"]
        if len(self.groups) == 0:
            raise ValueError("Invalid config file: 'groups' must contain at least one entry")

        changes_detected = self._load_leds(doc)
        if not changes_detected:
            print("No changes detected in LED configuration.")

        self._check_leds()  # Validate total number of LEDs and their indices
        print(f"Successfully (re)loaded config ('{self.name}') with {len(self.leds)} LEDs.")

    def _load_leds(self, doc: dict):
        changes_detected = False
        for idx, led_obj in enumerate(doc["leds"]):
            for key in self.ConfigLedKeys:
                if key not in list(led_obj.keys()):
                    raise ValueError(f"LED entry missing key: '{key}'")

            group = led_obj["group"]
            if group not in list(self.groups.keys()):
                raise ValueError(f"LED group '{group}' not defined in groups {list(self.groups.keys())}")

            pcb_idx = led_obj["pcb_idx"]
            led_idx = led_obj["led_idx"]
            correction = led_obj["correction"]
            brightness = self.groups[group] + correction
            if not (0 <= brightness <= MAX_BRIGHTNESS):
                raise ValueError(f"LED brightness ({brightness}) out of range [0, {MAX_BRIGHTNESS}]")

            led = Led(pcb_index=pcb_idx, led_index=led_idx, brightness=brightness)
            if len(self.leds) > idx:
                # Entry already exists from previous load, check for changes
                assert self.leds[idx].pcb_index == pcb_idx and self.leds[idx].led_index == led_idx
                if self.leds[idx].brightness != brightness:
                    changes_detected = True
                    self.leds[idx].brightness = brightness
                else:
                    continue  # No change, don't print
            else:
                changes_detected = True
                self.leds.append(led)
            print(
                f"\tLED({pcb_idx:02d},{led_idx:02d}): group='{group:3s}' correction={correction:02d} => brightness={brightness:03d}"
            )
        return changes_detected

    def _check_leds(self):
        for idx, led in enumerate(self.leds):
            pcb_index = (idx // LED_COUNT) + 1
            led_index = (idx % LED_COUNT) + 1
            if led.pcb_index != pcb_index or led.led_index != led_index:
                raise ValueError(
                    f"LED has duplicated/missing/unsorted index ({led.pcb_index},{led.led_index}), expected ({pcb_index},{led_index})"
                )

        if len(self.leds) != LED_TOTAL:
            raise ValueError(f"LEDs length {len(self.leds)} does not match expected {LED_TOTAL}")

    def get_leds(self) -> list[Led]:
        return self.leds


if __name__ == "__main__":
    dc = DaisyChain()
    dc.load_config("chain_config.json")
