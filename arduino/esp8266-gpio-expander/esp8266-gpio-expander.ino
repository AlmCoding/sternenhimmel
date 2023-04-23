#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MCP23017.h>
#include <Wire.h>
#include <math.h>
#include "SequenceParser.hpp"
#include "outputNum2McpPin.hpp"

#define DEBUG true  // Set true for debug output
#define DEBUG_SERIAL \
  if (DEBUG) Serial

#define SIMULATE_MCPS  // When no hardware available

#define MCP_COUNT 8

void set_outputs(uint16_t value, bool write = true);
void generate_outputs(gpio_expander::SequenceStep_t *step);
bool play_step(gpio_expander::SequenceStep_t *step, bool reset_fsm = false);
bool play_sequence(bool reset_fsm = false);
bool test_outputs(bool reset_fsm = false);

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

struct McpOutput {
  uint16_t value = 0;
  bool changed = true;
};

static McpOutput mcpOutputArray[MCP_COUNT];

static bool runTestFlag = false;
static bool runSequenceFlag = false;

/*
  test='http://192.168.4.1/test',
  sequence='http://192.168.4.1/seq?s=[[[1,2,3],0,1000,3],[[3,4],500,1000,2]]',
*/

void set_outputs(uint16_t value, bool write) {
  for (uint8_t idx = 0; idx < MCP_COUNT; idx++) {
    if (mcpOutputArray[idx].value != value) {
      mcpOutputArray[idx].value = value;
      mcpOutputArray[idx].changed = true;
    }
  }

  if (write == true) {
    write_mcps();
  }
}

void generate_outputs(uint8_t outputs[]) {
  uint8_t idx = 0;
  while (outputs[idx] != 0xff) {
    gpio_expander::McpPin_t mpin = gpio_expander::outputNum2McpPin(outputs[idx]);
    mcpOutputArray[mpin.chip_number].value |= 0x0001 << mpin.pin_number;
    mcpOutputArray[mpin.chip_number].changed = true;
    idx++;
  }
}

enum class PlayStepState {
  pause_entry,
  pause,
  pulse_entry,
  pulse,
};

bool play_step(gpio_expander::SequenceStep_t *step, bool reset_fsm) {
  static PlayStepState fsm_state = PlayStepState::pause_entry;
  static unsigned long start_time;
  static uint8_t reps_counter = 0;
  bool in_progress = true;

  if (reset_fsm == true) {
    // Reset step fsm
    DEBUG_SERIAL.println("Reset step fsm.");
    fsm_state = PlayStepState::pause_entry;
    reps_counter = 0;
    return false;
  }

  if (fsm_state == PlayStepState::pause_entry) {
    // Start pause time
    start_time = millis();

    // Clear all outputs (no write)
    set_outputs(0x0000, false);
    fsm_state = PlayStepState::pause;
  }

  if (fsm_state == PlayStepState::pause) {
    // Check pause time
    if ((millis() - start_time) >= step->offset) {
      fsm_state = PlayStepState::pulse_entry;
    }
  }

  if (fsm_state == PlayStepState::pulse_entry) {
    // Start pulse time
    start_time = millis();

    // Generate outputs
    generate_outputs(step->outputs);

    fsm_state = PlayStepState::pulse;
  }

  if (fsm_state == PlayStepState::pulse) {
    // Check pulse end
    if ((millis() - start_time) >= step->duration) {
      reps_counter++;

      if (reps_counter == step->reps) {
        // Step finised
        DEBUG_SERIAL.println("Step finished.");
        // Reset step
        play_step(nullptr, true);
        return false;
      }

      // Repeat step
      fsm_state = PlayStepState::pause_entry;
    }
  }

  // Write mcps
  write_mcps();

  return in_progress;
}

enum class PlaySeqState {
  idle,
  running,
};

bool play_sequence(bool reset_fsm) {
  static PlaySeqState fsm_state = PlaySeqState::idle;
  static gpio_expander::SequenceStep_t seq_step;
  bool in_progress = true;

  if (reset_fsm == true) {
    // Reset sequence fsm
    DEBUG_SERIAL.println("Reset sequence fsm.");
    fsm_state = PlaySeqState::idle;
    // Clear all outputs
    set_outputs(0x0000);
    return false;
  }

  if (fsm_state == PlaySeqState::idle) {
    // Parse next sequence step
    gpio_expander::ParserStatus status;
    status = seqParser.parseNextStep(&seq_step);

    if (status != gpio_expander::ParserStatus::OK) {
      // Sequence finised
      DEBUG_SERIAL.println("Sequence finished.");
      // Reset sequence fsm
      play_sequence(true);
      return false;
    }

    // Reset step fsm for new step
    play_step(nullptr, true);
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
  init,
  toggle,
  delay,
};

bool test_outputs(bool reset_fsm) {
  static TestStage test_stage = TestStage::init;
  static uint32_t output = 0xffff;
  static unsigned long start_time;
  static uint32_t toggle_counter = 0;
  bool in_progress = true;

  // Reset function state
  if (reset_fsm == true) {
    DEBUG_SERIAL.println("Reset test_outputs() function state.");
    test_stage = TestStage::init;
    output = 0xffff;
    toggle_counter = 0;
    return false;
  }

  // Start
  if (test_stage == TestStage::init) {
    DEBUG_SERIAL.println(DIVIDER);
    DEBUG_SERIAL.println("Test Outputs [...]");
    test_stage = TestStage::toggle;
  }

  // Write gpios
  if (test_stage == TestStage::toggle) {
    output = ~output;
    set_outputs(output);
    start_time = millis();
    test_stage = TestStage::delay;
    toggle_counter++;
  }

  // Delay of 300 ms
  if (test_stage == TestStage::delay) {
    if ((millis() - start_time) > 300) {
      test_stage = TestStage::toggle;
    }
  }

  // End condition
  if (toggle_counter > 32) {
    DEBUG_SERIAL.println("Test Outputs [OK]");
    test_stage = TestStage::init;
    toggle_counter = 0;
    set_outputs(0x0000);
    return false;
  }

  return in_progress;
}

void handleRoot() {
  char json_response[] = "{\"test\":\"http://192.168.4.1/tst\",\"sequence\":\"http://192.168.4.1/seq?s=[[[1,2],0,1000,3],[[2,3],0,1000,3]]\"}";
  server.send(200, "application/json", json_response);
}

void handleTest() {
  server.send(200, "text/html", "<h1>Self test running ...</h1>");

  // Reset test_outputs function state
  test_outputs(true);

  runTestFlag = true;
}

void handleSequence() {
  server.send(200, "text/html", "<h1>Sequence is running ...</h1>");
  seqString = server.argName(0);

  // Init sequence parser
  seqParser.init(seqString.c_str());

  // Reset play_sequence function state
  play_sequence(true);

  runSequenceFlag = true;
}

void write_mcps() {
  for (uint8_t idx = 0; idx < MCP_COUNT; idx++) {
    if (mcpOutputArray[idx].changed == true) {
      uint16_t value = mcpOutputArray[idx].value;

#ifdef SIMULATE_MCPS
      char buffer[32];
      sprintf(buffer, "Mcp: %d, val: 0x%04X", idx, value);
      DEBUG_SERIAL.println(buffer);
#else
      MCP23017 *mcp = &mcp_array[idx];
      mcp->write(value);
      mcpOutputArray[idx].changed = false;
#endif

      // uint8_t A = mcp_output_array[idx];
      // uint8_t B = mcp_output_array[idx] >> 8;
      // mcp->writePort(MCP23017Port::A, A);
      // mcp->writePort(MCP23017Port::B, B);
    }
  }
}

void setup_mcps() {
  DEBUG_SERIAL.println(DIVIDER);
  DEBUG_SERIAL.println("Setup GPIO Expanders [...]");

#ifndef SIMULATE_MCPS
  // Clear MCP23017 I2C address MSB pin
  DEBUG_SERIAL.println("Clear MCP23017 I2C address MSB pin.");
  int address_pin = 16;
  pinMode(address_pin, OUTPUT);
  digitalWrite(address_pin, LOW);

  // Init MCP23017s
  DEBUG_SERIAL.println("Initialize MCP23017 ports as output.");
  MCP23017 *mcp = mcp_array;
  for (int i = 0; i < MCP_COUNT; i++) {
    mcp->init();
    mcp->portMode(MCP23017Port::A, 0, 0, 0);  // Port A as output
    mcp->portMode(MCP23017Port::B, 0, 0, 0);  // Port B as output
    mcp++;
  }
#endif

  // Clear GPIOs
  DEBUG_SERIAL.println("Clear GPIOs.");
  set_outputs(0x0000);

  DEBUG_SERIAL.println("Setup GPIO Expanders [OK]");
}

void setup_access_point() {
  DEBUG_SERIAL.println(DIVIDER);
  DEBUG_SERIAL.println("Setup Access Point [...]");
  DEBUG_SERIAL.print("SSID: ");
  DEBUG_SERIAL.println(ssid);
  DEBUG_SERIAL.print("Password: ");
  DEBUG_SERIAL.println(password);

  WiFi.softAP(ssid, password);
  IPAddress ip = WiFi.softAPIP();
  DEBUG_SERIAL.print("IP address: ");
  DEBUG_SERIAL.println(ip);

  server.on("/", handleRoot);
  server.on("/tst", handleTest);
  server.on("/seq", handleSequence);
  server.begin();

  DEBUG_SERIAL.println("HTTP server started.");
  DEBUG_SERIAL.println("Setup Access Point [OK]");
}

void setup() {
  // Setup Serial and I2C Interface
  DEBUG_SERIAL.begin(9600);
  Wire.begin();

  // Setup MCP23017 group
  setup_mcps();

  // Setup Wifi Access Point
  setup_access_point();
}

void loop() {
  server.handleClient();

  if (runTestFlag == true) {
    runTestFlag = test_outputs();
  }

  if (runSequenceFlag == true) {
    runSequenceFlag = play_sequence();
  }
}
