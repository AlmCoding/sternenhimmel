#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MCP23017.h>
#include <Wire.h>
#include <math.h>


const char *DIVIDER = "<=====================================>";
const char *ssid = "Sternenhimmel";
const char *password = "Sternenhimmel";

ESP8266WebServer server(80);

const int mcp_array_len = 8;
MCP23017 mcp_array[8] = {
  MCP23017(0x20),
  MCP23017(0x21),
  MCP23017(0x22),
  MCP23017(0x23),
  MCP23017(0x24),
  MCP23017(0x25),
  MCP23017(0x26),
  MCP23017(0x27)
};

unsigned int mcp_output_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int outputs[32] = {-1};

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
  for(int i=0;i<output_string.length();i++) {
    if(output_string[i] == ',') {
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


int translate_output(int number, int result[])  {
  /**
   * number is range of [1..120]
   * result[0] is range of [0..7]
   * result[1] is range of [0..15]
   */
  
  // Reverse number
  number = 121 - number;
  //Serial.println(String(number));

  unsigned int mcp_offset = 0;
  if (number > 60) {
    number -= 60;
    mcp_offset = 4;
  }

  int rj45_connector_number = ceil(float(number) / 6);
  //Serial.println(String(rj45_connector_number));
  int rj45_connector_pin = number - (rj45_connector_number-1)*6;
  //Serial.println(String(rj45_connector_pin));

  int range[] = {6,5,4,3,2,1};
  int pseudo_rj_45_connector_pin = range[rj45_connector_pin-1];
  //Serial.println(String(pseudo_rj_45_connector_pin));
  int pseudo_pin_number = (rj45_connector_number - 1) * 6 + pseudo_rj_45_connector_pin;
  //Serial.println(String(pseudo_pin_number));
  
  int mcp_number = ceil(float(pseudo_pin_number) / 16);
  int pin_number = pseudo_pin_number - (mcp_number - 1) * 16;
  //Serial.println(String(mcp_number) + " <-> " + String(pin_number));
  result[0] = mcp_offset + mcp_number - 1;
  result[1] = pin_number - 1;
}


enum PlaybackStages {
  _idle,
  _offset,
  _pulse,
  _pause,
  _stay,
};

bool playback_sequence(bool run_function=true) {
  static PlaybackStages play_show_stage = _idle;
  static unsigned long start_time;
  static bool do_once = true;
  static int pulse_counter = 0;

  // Reset function state
  if(run_function == false) {
    Serial.println("Reset play_show() function state.");
    play_show_stage = _idle;
    do_once = true;
    pulse_counter = 0;
    return false;
  }

  // Start
  if(play_show_stage == _idle) {
    Serial.println(DIVIDER);
    Serial.println("Play Show [...]");
    pulse_counter = 0;
    play_show_stage = _offset;
  }

  // Offset
  if(play_show_stage == _offset) {
    if(do_once == true) {
      Serial.println("Offset.");
      write_mcps(0x0000);
      start_time = millis();
      do_once = false;
    } else {
      if((millis() - start_time) > timings.OFFSET) {
        play_show_stage = _pulse;
        do_once = true;
      }
    }
  }

  // Pulse
  if(play_show_stage == _pulse) {
    if(do_once == true) {
      Serial.println("Set Pulse.");
      write_outputs();
      start_time = millis();
      pulse_counter++;
      do_once = false;
    } else {
      if((millis() - start_time) > timings.PULSE) {
        play_show_stage = _pause;
        do_once = true;
      }
    }
  }

  // Pause
  if(play_show_stage == _pause) {
    if(do_once == true) {
      Serial.println("Reset Pulse.");
      write_mcps(0x0000);
      start_time = millis();
      do_once = false;
    } else {
      if((millis() - start_time) > timings.PAUSE) {
        if(pulse_counter >= timings.COUNT) {
          play_show_stage = _stay;
        } else {
          play_show_stage = _pulse;
        }
        do_once = true;
      }
    }
  }

  // Stay
  if(play_show_stage == _stay) {
    if(do_once == true) {
      Serial.println("Stay.");
      write_outputs();
      start_time = millis();
      do_once = false;
    } else {
      if((millis() - start_time) > timings.STAY) {
        write_mcps(0x0000);
        play_show_stage = _idle;
        do_once = true;
        return false;
      }
    }
  }

  return true;
}


enum TestStages {
  one,
  two,
  three
};

bool test_outputs(bool run_function=true) {
  static TestStages tets_gpio_stage = one;
  static unsigned int output = 0xffff;
  static unsigned long start_time;
  static int toggle_counter = 0;

  // Reset function state
  if(run_function == false) {
    Serial.println("Reset test_outputs() function state.");
    tets_gpio_stage = one;
    output = 0xffff;
    toggle_counter = 0;
    return false;
  }

  // Start
  if(tets_gpio_stage == one) {
    Serial.println(DIVIDER);
    Serial.println("Test Outputs [...]");
    tets_gpio_stage = two;
  }

  // Write gpios
  if(tets_gpio_stage == two) {
    output = ~output;
    write_mcps(output);
    start_time = millis();
    tets_gpio_stage = three;
    toggle_counter++;
  }

  // Delay of 300 ms
  if(tets_gpio_stage == three) {
    if((millis() - start_time) > 300) {
      tets_gpio_stage = two;
    }
  }
  
  // End condition
  if(toggle_counter > 32) {
    Serial.println("Test Outputs [OK]");
    tets_gpio_stage = one;
    toggle_counter = 0;
    write_mcps(0x0000);
    return false;
  }
  
  return true;
}


void write_mcps(unsigned int value) {
  byte A = value;
  byte B = value >> 8;

  MCP23017 *mcp = mcp_array;
  for(int i=0;i<mcp_array_len;i++) {
    mcp->init();
    mcp->writeRegister(MCP23017_REGISTER::GPIOA, A);
    mcp->writeRegister(MCP23017_REGISTER::GPIOB, B);
    mcp_output_array[i] = (B << 8) | A;
    mcp++;
  }
}


void write_outputs() {
  unsigned int i = 0;
  while(1) {
    if(outputs[i] == -1) {
      break;
    }
    Serial.println(outputs[i]);

    int result[2];
    translate_output(outputs[i], result);
    int mcp_number = result[0];
    MCP23017 mcp = mcp_array[mcp_number];
    unsigned int value = 0x0001 << result[1];

    mcp_output_array[mcp_number] |= value;

    byte A = mcp_output_array[mcp_number];
    byte B = mcp_output_array[mcp_number] >> 8;
    mcp.writeRegister(MCP23017_REGISTER::GPIOA, A);
    mcp.writeRegister(MCP23017_REGISTER::GPIOB, B);

    i++;
  }
}

void handleRoot() {
  char json_response[] = "{\"test\":\"http://192.168.4.1/test\",\"output\":\"http://192.168.4.1/output?number=1,2,3\",\"timing\":\"http://192.168.4.1/timing?offset=500&pulse=900&pause=500&count=4&stay=16000\"}";
  server.send(200, "application/json", json_response);
}


void handleTest() {
  server.send(200, "text/html", "<h1>Self test running ...</h1>");
  handleTest_flag = true;
  // Reset test_outputs function state
  test_outputs(false);
}


void handleTiming() {
  // timing_example='10.0.0.1:80/timing?offset=0.5&pulse=0.9&pause=0.5&count=4&stay=16.0'
  Serial.println(DIVIDER);
  Serial.println("Set Timings [...]");
  
  Serial.println("Request arguments: ");
  for(int i=0;i<server.args();i++) {
    Serial.println(" - " + server.argName(i) + " = " + server.arg(i));
    if(server.argName(i) == "offset") {
      timings.OFFSET = server.arg(i).toInt();
    } else if(server.argName(i) == "pulse") {
      timings.PULSE = server.arg(i).toInt();
    } else if(server.argName(i) == "pause") {
      timings.PAUSE = server.arg(i).toInt();
    } else if(server.argName(i) == "count") {
      timings.COUNT = server.arg(i).toInt();
    } else if(server.argName(i) == "stay") {
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
  for(int i=0;i<server.args();i++) {
    Serial.println(" - " + server.argName(i) + " = " + server.arg(i));
    if(server.argName(i) == "number") {
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
  for(int i=0;i<mcp_array_len;i++) {
    mcp->init();
    mcp->portMode(MCP23017_PORT::A, 0);  // Port A as output
    mcp->portMode(MCP23017_PORT::B, 0);  // Port B as output
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
  server.on("/test", handleTest);
  server.on("/output", handleOutput);
  server.on("/timing", handleTiming);
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

  if(handleTest_flag == true) {
    handleTest_flag = test_outputs();
  }

  if(handleOutput_flag == true) {
    handleOutput_flag = playback_sequence();
  }
}
