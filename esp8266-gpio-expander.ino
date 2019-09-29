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


bool handleRoot_flag = false;
bool handleTest_flag = false;
bool handleShow_flag = false;

unsigned int show_number = 0;

struct TIMING {
  int OFFSET;
  int PULSE;
  int PAUSE;
  int COUNT;
  int STAY;
};

TIMING timings;

enum Stages {
  one,
  two,
  three,
  four,
  five
};


/*
    test_example='10.0.0.1:80/test',
    show_example='10.0.0.1:80/show?number=3',
    output_example='10.0.0.1:80/output?number=3,4',
    timing_example='10.0.0.1:80/timing?offset=0.5&pulse=0.9&pause=0.5&count=4&stay=16.0'
*/


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
  Serial.println(String(mcp_number) + " <-> " + String(pin_number));
  result[0] = mcp_offset + mcp_number-1;
  result[1] = pin_number-1;
}
        

bool play_show(bool run_function=true) {
  static Stages play_show_stage = one;

  // Reset function state
  if(run_function == false) {
    Serial.println("Reset play_show() function state.");
    play_show_stage = one;
    return false;
  }
}


bool test_gpios(bool run_function=true) {
  static Stages tets_gpio_stage = one;
  static byte output = 0b10101010;
  static unsigned long start_time;
  static int toggle_counter = 0;

  // Reset function state
  if(run_function == false) {
    Serial.println("Reset test_gpios() function state.");
    tets_gpio_stage = one;
    output = 0b11111111;
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
  test_gpios(false);
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
  Serial.println(DIVIDER);
  Serial.println("Get Show [...]");
  
  Serial.println("Request arguments: ");
  for(int i=0;i<server.args();i++) {
    Serial.println(" - " + server.argName(i) + " = " + server.arg(i));
    if(server.argName(i) == "number") {
      show_number = server.arg(i).toInt();
        handleShow_flag = true;
        // Reset play_show function state
        play_show(false);
    }
  }
  Serial.println("Get Show [OK]");
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

  /*
  for(int i=1; i<=120; i++) {
    int result[2];
    translate_output(i, result);

    MCP23017 mcp = mcp_array[result[0]];
    unsigned int value = 0x0001 << result[1];
    byte A = value;
    byte B = value >> 8;
    mcp.writeRegister(MCP23017_REGISTER::GPIOA, A);
    mcp.writeRegister(MCP23017_REGISTER::GPIOB, B);
    delay(300);
    write_gpios(0x00);
  }

   while(1);
   */

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
