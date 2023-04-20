#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MCP23017.h>
#include <Wire.h>
#include <math.h>

#define MCP_COUNT 8

const char *DIVIDER = "<=====================================>";
const char *ssid = "Sternenhimmel";
const char *password = "Sternenhimmel3";

ESP8266WebServer server(80);

MCP23017 mcp_array[MCP_COUNT] = {
  MCP23017(0x20),
  MCP23017(0x21),
  MCP23017(0x22),
  MCP23017(0x23),
  MCP23017(0x24),
  MCP23017(0x25),
  MCP23017(0x26),
  MCP23017(0x27)
};

struct ExpanderPin_t {
  uint32_t chip_number;
  uint32_t pin_number;
};

uint32_t mcp_output_array[8] = { 0 };
bool mcp_output_update[8] = { true };
int32_t outputs[32] = { -1 };

bool handleRoot_flag = false;
bool handleTest_flag = false;
bool handleOutput_flag = false;

struct TIMING {
  int OFFSET;
  int PULSE;
  int PAUSE;
  int COUNT;
  int STAY;
};
TIMING timings;

/*
  test='http://192.168.4.1/test',
  output='http://192.168.4.1/output?number=3,4,',
  timing='http://192.168.4.1/timing?offset=500&pulse=900&pause=500&count=4&stay=16000'
*/

void text2outputs(String output_string) {
  Serial.println(output_string);
  output_string += ',';
  int start_idx = 0;
  int end_idx = 0;
  int idx = 0;
  for (int i = 0; i < output_string.length(); i++) {
    if (output_string[i] == ',') {
      end_idx = i;
      String element = output_string.substring(start_idx, end_idx);
      Serial.println(element);
      start_idx = i + 1;
      outputs[idx] = element.toInt();
      idx++;
    }
  }
  outputs[idx] = -1;
}

ExpanderPin_t translate_output(int32_t output_number) {
  /**
   * output_number [1..120]
   */

  // Connector shift
  if (output_number <= 108) {
    output_number += 6;
  } else if (output_number <= 114) {
    output_number -= 108;
  }

  // Reverse number
  output_number = 121 - output_number;

  uint32_t mcp_offset = 0;
  if (output_number > 60) {
    output_number -= 60;
    mcp_offset = 4;
  }

  int32_t rj45_connector_number = output_number / 6;
  if ((output_number % 6) > 0) {
    output_number++;
  }
  int32_t rj45_connector_pin = output_number - (rj45_connector_number - 1) * 6;

  int32_t range[] = { 6, 5, 4, 3, 2, 1 };
  int32_t pseudo_rj_45_connector_pin = range[rj45_connector_pin - 1];
  int32_t pseudo_pin_number = (rj45_connector_number - 1) * 6 + pseudo_rj_45_connector_pin;

  int32_t mcp_number = pseudo_pin_number / 16;
  if ((pseudo_pin_number % 16) > 0) {
    pseudo_pin_number++;
  }
  int32_t pin_number = pseudo_pin_number - (mcp_number - 1) * 16;

  ExpanderPin_t expander_pin;
  expander_pin.chip_number = mcp_offset + mcp_number - 1;
  expander_pin.pin_number = pin_number - 1;

  return expander_pin;
}


enum PlaybackStages {
  _idle,
  _offset,
  _pulse,
  _pause,
  _stay,
};

bool playback_sequence(bool run_function = true) {
  static PlaybackStages play_show_stage = _idle;
  static unsigned long start_time;
  static bool do_once = true;
  static int pulse_counter = 0;

  // Reset function state
  if (run_function == false) {
    Serial.println("Reset play_show() function state.");
    play_show_stage = _idle;
    do_once = true;
    pulse_counter = 0;
    return false;
  }

  // Start
  if (play_show_stage == _idle) {
    Serial.println(DIVIDER);
    Serial.println("Play Show [...]");
    pulse_counter = 0;
    play_show_stage = _offset;
  }

  // Offset
  if (play_show_stage == _offset) {
    if (do_once == true) {
      Serial.println("Offset.");
      write_mcps(0x0000);
      start_time = millis();
      do_once = false;
    } else {
      if ((millis() - start_time) > timings.OFFSET) {
        play_show_stage = _pulse;
        do_once = true;
      }
    }
  }

  // Pulse
  if (play_show_stage == _pulse) {
    if (do_once == true) {
      Serial.println("Set Pulse.");
      write_outputs();
      start_time = millis();
      pulse_counter++;
      do_once = false;
    } else {
      if ((millis() - start_time) > timings.PULSE) {
        play_show_stage = _pause;
        do_once = true;
      }
    }
  }

  // Pause
  if (play_show_stage == _pause) {
    if (do_once == true) {
      Serial.println("Reset Pulse.");
      write_mcps(0x0000);
      start_time = millis();
      do_once = false;
    } else {
      if ((millis() - start_time) > timings.PAUSE) {
        if (pulse_counter >= timings.COUNT) {
          play_show_stage = _stay;
        } else {
          play_show_stage = _pulse;
        }
        do_once = true;
      }
    }
  }

  // Stay
  if (play_show_stage == _stay) {
    if (do_once == true) {
      Serial.println("Stay.");
      write_outputs();
      start_time = millis();
      do_once = false;
    } else {
      if ((millis() - start_time) > timings.STAY) {
        write_mcps(0x0000);
        play_show_stage = _idle;
        do_once = true;
        return false;
      }
    }
  }

  return true;
}

enum class TestStage {
  one,
  two,
  three
};

bool test_outputs(bool run_function = true) {
  static TestStage test_stage = TestStage::one;
  static uint32_t output = 0xffff;
  static unsigned long start_time;
  static uint32_t toggle_counter = 0;

  // Reset function state
  if (run_function == false) {
    Serial.println("Reset test_outputs() function state.");
    test_stage = TestStage::one;
    output = 0xffff;
    toggle_counter = 0;
    return false;
  }

  // Start
  if (test_stage == TestStage::one) {
    Serial.println(DIVIDER);
    Serial.println("Test Outputs [...]");
    test_stage = TestStage::two;
  }

  // Write gpios
  if (test_stage == TestStage::two) {
    output = ~output;
    write_mcps(output);
    start_time = millis();
    test_stage = TestStage::three;
    toggle_counter++;
  }

  // Delay of 300 ms
  if (test_stage == TestStage::three) {
    if ((millis() - start_time) > 300) {
      test_stage = TestStage::two;
    }
  }

  // End condition
  if (toggle_counter > 32) {
    Serial.println("Test Outputs [OK]");
    test_stage = TestStage::one;
    toggle_counter = 0;
    write_mcps(0x0000);
    return false;
  }

  return true;
}

void write_mcps(uint32_t value) {
  uint8_t A = value;
  uint8_t B = value >> 8;

  for (int idx = 0; idx < MCP_COUNT; idx++) {
    MCP23017 *mcp = &mcp_array[idx];
    // mcp->init();
    mcp->writePort(MCP23017Port::A, A);
    mcp->writePort(MCP23017Port::B, B);
    mcp_output_array[idx] = value;
  }
}

void write_outputs() {
  uint32_t i = 0;

  while (outputs[i] != -1) {
    Serial.println(outputs[i]);

    ExpanderPin_t exp_pin = translate_output(outputs[i]);
    uint32_t mcp_number = exp_pin.chip_number;
    uint32_t value = 0x0001 << exp_pin.pin_number;

    mcp_output_array[mcp_number] |= value;
    mcp_output_update[mcp_number] = true;

    i++;
  }

  for (int idx = 0; idx < MCP_COUNT; idx++) {
    if (mcp_output_update[idx] == true) {
      uint8_t A = mcp_output_array[idx];
      uint8_t B = mcp_output_array[idx] >> 8;

      MCP23017 *mcp = &mcp_array[idx];
      mcp->writePort(MCP23017Port::A, A);
      mcp->writePort(MCP23017Port::B, B);

      mcp_output_update[i] = false;
    }
  }
}

void handleRoot() {
  char json_response[] = "{\"test\":\"http://192.168.4.1/test\",\"output\":\"http://192.168.4.1/output?number=1,2,3\",\"timing\":\"http://192.168.4.1/timing?offset=500&pulse=900&pause=500&count=4&stay=16000\"}";
  server.send(200, "application/json", json_response);
}


void handleTest() {
  server.send(200, "text/html", "<h1>Self test running ...</h1>");

  // Reset test_outputs function state
  test_outputs(false);

  handleTest_flag = true;
}


void handleTiming() {
  // timing_example='10.0.0.1:80/timing?offset=0.5&pulse=0.9&pause=0.5&count=4&stay=16.0'
  Serial.println(DIVIDER);
  Serial.println("Set Timings [...]");

  Serial.println("Request arguments: ");
  for (int i = 0; i < server.args(); i++) {
    Serial.println(" - " + server.argName(i) + " = " + server.arg(i));
    if (server.argName(i) == "offset") {
      timings.OFFSET = server.arg(i).toInt();
    } else if (server.argName(i) == "pulse") {
      timings.PULSE = server.arg(i).toInt();
    } else if (server.argName(i) == "pause") {
      timings.PAUSE = server.arg(i).toInt();
    } else if (server.argName(i) == "count") {
      timings.COUNT = server.arg(i).toInt();
    } else if (server.argName(i) == "stay") {
      timings.STAY = server.arg(i).toInt();
    }
  }

  String response = "offset: " + String(timings.OFFSET) + ", ";
  response += "pulse: " + String(timings.PULSE) + ", ";
  response += "pause: " + String(timings.PAUSE) + ", ";
  response += "count: " + String(timings.COUNT) + ", ";
  response += "stay: " + String(timings.STAY);

  Serial.println(response);
  Serial.println("Set Timings [OK]");
  server.send(200, "text/html", response);
}


void handleOutput() {
  Serial.println(DIVIDER);
  Serial.println("Playback [...]");

  Serial.println("Request arguments: ");
  for (int i = 0; i < server.args(); i++) {
    Serial.println(" - " + server.argName(i) + " = " + server.arg(i));
    if (server.argName(i) == "number") {
      String value = server.arg(i);
      text2outputs(value);
      handleOutput_flag = true;
      // Reset playback_sequence function state
      playback_sequence(false);
    }
  }
  Serial.println("Playback [OK]");
  server.send(200, "Ok.");
}


void setup_gpio_expanders() {
  Serial.println(DIVIDER);
  Serial.println("Setup GPIO Expanders [...]");

  // Clear MCP23017 I2C address MSB pin
  Serial.println("Clear MCP23017 I2C address MSB pin.");
  int address_pin = 16;
  pinMode(address_pin, OUTPUT);
  digitalWrite(address_pin, LOW);

  // Init MCP23017s
  Serial.println("Initialize MCP23017 ports as output.");
  MCP23017 *mcp = mcp_array;
  for (int i = 0; i < MCP_COUNT; i++) {
    mcp->init();
    mcp->portMode(MCP23017Port::A, 0, 0, 0);  // Port A as output
    mcp->portMode(MCP23017Port::B, 0, 0, 0);  // Port B as output
    mcp++;
  }

  // Clear GPIOs
  Serial.println("Clear GPIOs.");
  write_mcps(0x0000);

  Serial.println("Setup GPIO Expanders [OK]");
}


void setup_access_point() {
  Serial.println(DIVIDER);
  Serial.println("Setup Access Point [...]");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);

  WiFi.softAP(ssid, password);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(ip);

  server.on("/", handleRoot);
  server.on("/tst", handleTest);
  server.on("/seq", handleSequence);
  // server.on("/output", handleOutput);
  // server.on("/timing", handleTiming);
  server.begin();

  Serial.println("HTTP server started.");
  Serial.println("Setup Access Point [OK]");
}

void setup() {
  // Setup Serial and I2C Interface
  Serial.begin(9600);
  Wire.begin();

  // Init timings struct with default values
  timings.OFFSET = 500;
  timings.PULSE = 900;
  timings.PAUSE = 500;
  timings.COUNT = 4;
  timings.STAY = 16000;

  // Setup MCP23017 gpio expanders
  setup_gpio_expanders();

  // Setup Wifi Access Point
  setup_access_point();
}

void loop() {
  server.handleClient();

  if (handleTest_flag == true) {
    handleTest_flag = test_outputs();
  }

  if (handleOutput_flag == true) {
    handleOutput_flag = playback_sequence();
  }
}
