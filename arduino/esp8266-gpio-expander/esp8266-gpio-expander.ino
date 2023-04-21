#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MCP23017.h>
#include <Wire.h>
#include <math.h>
#include "SequenceParser.hpp"
#include "outputNum2McpPin.hpp"

#define MCP_COUNT 8

static const char *DIVIDER = "<=====================================>";
static const char *ssid = "Sternenhimmel";
static const char *password = "Sternenhimmel3";

static ESP8266WebServer server{ 80 };

static String seqString;
static gpio_expander::SequenceParser seqParser{};

static MCP23017 mcp_array[MCP_COUNT] = {
  MCP23017(0x20),
  MCP23017(0x21),
  MCP23017(0x22),
  MCP23017(0x23),
  MCP23017(0x24),
  MCP23017(0x25),
  MCP23017(0x26),
  MCP23017(0x27)
};

static uint32_t mcp_output_array[8] = { 0 };
static bool mcp_output_update[8] = { true };

static bool handleTest_flag = false;
static bool handleSequence_flag = false;

/*
  test='http://192.168.4.1/test',
  sequence='http://192.168.4.1/seq?s=[[[1,2,3],0,1000,3],[[3,4],100,1000,2]]',
*/

enum class PlayStepState {
  pause_entry,
  pause,
  pulse_entry,
  pulse,
};

bool play_step(gpio_expander::SequenceStep_t *step, bool reset_fsm = false) {
  static PlayStepState fsm_state = PlayStepState::pause_entry;
  static unsigned long start_time;
  static uint8_t rep_counter = 0;
  bool in_progress = true;

  if (reset_fsm == true) {
    // Reset step fsm
    Serial.println("Reset step fsm.");
    fsm_state = PlayStepState::pause_entry;
    return false;
  }

  if (fsm_state == PlayStepState::pause_entry) {
    start_time = millis();

    // Clear all outputs
    // ...
    fsm_state = PlayStepState::pause;
  }

  if (fsm_state == PlayStepState::pause) {
    // Check pause end
    if ((millis() - start_time) >= step->offset) {
      fsm_state = PlayStepState::pulse_entry;
    }
  }

  if (fsm_state == PlayStepState::pulse_entry) {
    // Write outputs
    // ...
    fsm_state = PlayStepState::pulse;
  }

  if (fsm_state == PlayStepState::pulse) {
    // Check pulse end
    if ((millis() - start_time) >= step->duration) {
      return false;
    }
  }

  // Update mcps
  update_mcps()

  return in_progress;
}

enum class PlaySeqState {
  idle,
  running,
};

bool play_sequence(bool reset_fsm = false) {
  static PlaySeqState fsm_state = PlaySeqState::idle;
  static gpio_expander::SequenceStep_t seq_step;
  bool in_progress = true;

  if (reset_fsm == true) {
    // Reset sequence fsm
    Serial.println("Reset sequence fsm.");
    // Reset step fsm
    play_step(nullptr, true);
    fsm_state = PlaySeqState::idle;
    return false;
  }

  if (fsm_state == PlaySeqState::idle) {
    // Parse next sequence step
    gpio_expander::ParserStatus status;
    status = seqParser.parseNextStep(&seq_step);

    if (status != gpio_expander::ParserStatus::OK) {
      // Sequence finised
      Serial.println("Sequence finished.");
      fsm_state = PlaySeqState::idle;
      // clear all outputs
      return false;
    }
  }

  // Play sequence step
  if (play_step(&seq_step) == true) {
    // Step in progress
    fsm_state = PlaySeqState::running;
  } else {
    // Step finished
    fsm_state = PlaySeqState::idle;
  }

  return in_progress;
}

enum class TestStage {
  one,
  two,
  three,
};

bool test_outputs(bool reset_fsm = false) {
  static TestStage test_stage = TestStage::one;
  static uint32_t output = 0xffff;
  static unsigned long start_time;
  static uint32_t toggle_counter = 0;
  bool in_progress = true;

  // Reset function state
  if (reset_fsm == true) {
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

  return in_progress;
}

void write_mcps(uint32_t value) {
  // uint8_t A = value;
  // uint8_t B = value >> 8;

  for (int idx = 0; idx < MCP_COUNT; idx++) {
    MCP23017 *mcp = &mcp_array[idx];
    // mcp->init();
    // mcp->writePort(MCP23017Port::A, A);
    // mcp->writePort(MCP23017Port::B, B);
    mcp->write(value);
    mcp_output_array[idx] = value;
  }
}

void update_mcps() {
  for (int idx = 0; idx < MCP_COUNT; idx++) {
    if (mcp_output_update[idx] == true) {
      MCP23017 *mcp = &mcp_array[idx];
      // uint8_t A = mcp_output_array[idx];
      // uint8_t B = mcp_output_array[idx] >> 8;
      // mcp->writePort(MCP23017Port::A, A);
      // mcp->writePort(MCP23017Port::B, B);
      mcp->write(mcp_output_array[idx]);
      mcp_output_update[idx] = false;
    }
  }
}

void handleRoot() {
  char json_response[] = "{\"test\":\"http://192.168.4.1/test\",\"output\":\"http://192.168.4.1/output?number=1,2,3\"}";
  server.send(200, "application/json", json_response);
}

void handleTest() {
  server.send(200, "text/html", "<h1>Self test running ...</h1>");

  // Reset test_outputs function state
  test_outputs(true);

  handleTest_flag = true;
}

void handleSequence() {
  server.send(200, "text/html", "<h1>Sequence is running ...</h1>");
  seqString = server.argName(0);

  // Init sequence parser
  seqParser.init(seqString.c_str());

  // Reset play_sequence function state
  play_sequence(true);

  handleSequence_flag = true;
}

/*
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
*/

/*
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
*/

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
  server.begin();

  Serial.println("HTTP server started.");
  Serial.println("Setup Access Point [OK]");
}

void setup() {
  // Setup Serial and I2C Interface
  Serial.begin(9600);
  Wire.begin();

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

  if (handleSequence_flag == true) {
    handleSequence_flag = play_sequence();
  }
}
