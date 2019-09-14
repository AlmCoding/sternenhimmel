#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MCP23017.h>
#include <Wire.h>


//IPAddress ip(10, 0, 0, 1); 
const char *ssid = "Sternenhimmel";
const char *password = "Sternenhimmel";

const char *DIVIDER = "<=====================================>";

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


void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected.</h1>");
}


void handleTest() {
  server.send(200, "text/html", "<h1>Self test running ...</h1>");
}


void handleAPI() {
  server.send(200, "text/html", "<h1>Ok.</h1>");
}


void test_gpios() {
  Serial.println("Test GPIOs ...");
  for(int i=0;i<32;i++) {
    write_gpios(0b10101010);
    delay(300);
    write_gpios(0b01010101);
    delay(300);
  }
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
  server.on("/api", handleAPI);
  server.on("/test", handleTest);
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
}
