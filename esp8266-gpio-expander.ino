#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MCP23017.h>
#include <Wire.h>


const char *DIVIDER = "<=====================================>";
const char *ssid = "Sternenhimmel";
const char *password = "Sternenhimmel";

ESP8266WebServer server(80);

const int mcp_array_len = 1;
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


bool handleRoot_flag = false;
bool handleTest_flag = false;
bool handleShow_flag = false;


struct TIMING {
  int OFFSET;
  int PULSE;
  int PAUSE;
  int COUNT;
  int STAY;
};

TIMING timings;


/*
    test_example='10.0.0.1:80/test',
    show_example='10.0.0.1:80/show?number=3',
    output_example='10.0.0.1:80/output?number=3,4',
    timing_example='10.0.0.1:80/timing?offset=0.5&pulse=0.9&pause=0.5&count=4&stay=16.0'
*/


bool play_show(bool reset_state=false) {
  
}


enum Stages {
  one,
  two,
  three,
  four,
  five
};

bool test_gpios(bool reset_state=false) {
  static Stages tets_gpio_stage = one;
  static byte output = 0b10101010;
  static unsigned long start_time;
  static int toggle_counter = 0;

  // Reset function state
  if(reset_state == true) {
    Serial.println(DIVIDER);
    Serial.println("Reset [test_gpios] function state.");
    Serial.println(DIVIDER);
    tets_gpio_stage = one;
    output = 0b10101010;
    toggle_counter = 0;
    return false;
  }

  // Start
  if(tets_gpio_stage == one) {
    Serial.println(DIVIDER);
    Serial.println("Test GPIOs [...]");
    tets_gpio_stage = two;
  }

  // Write gpios
  if(tets_gpio_stage == two) {
    output = (byte) ~output;
    write_gpios(output);
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
    Serial.println("Test GPIOs [OK]");
    tets_gpio_stage = one;
    toggle_counter = 0;
    write_gpios(0x00);
    return false;
  }
  
  return true;
}


void write_gpios(byte value) {
  MCP23017 *mcp = mcp_array;
  for(int i=0;i<mcp_array_len;i++) {
    mcp->init();
    mcp->writeRegister(MCP23017_REGISTER::GPIOA, value);
    mcp->writeRegister(MCP23017_REGISTER::GPIOB, value);
    mcp++;
  }
}


void handleRoot() {
  char json_response[] = "{\"test_example\":\"10.0.0.1:80/test\",\"show_example\":\"10.0.0.1:80/show?number=3\",\"output_example\":\"10.0.0.1:80/output?number=3,4\",\"timing_example\":\"10.0.0.1:80/timing?offset=0.5&pulse=0.9&pause=0.5&count=4&stay=16.0\"}"; 
  server.send(200, "application/json", json_response);
}


void handleTest() {
  server.send(200, "text/html", "<h1>Self test running ...</h1>");
  handleTest_flag = true;
  // Reset test_gpios function state
  test_gpios(true);
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


void handleShow() {
  server.send(200, "text/html", "<h1>Ok.</h1>");
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
  write_gpios(0x00);

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
  server.on("/show", handleShow);
  server.on("/timing", handleTiming);
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

  if(handleTest_flag == true) {
    handleTest_flag = test_gpios();
  }

  if(handleShow_flag == true) {
    handleShow_flag = play_show();
  }
    
}
