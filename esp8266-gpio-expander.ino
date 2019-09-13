#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h> 
#include <MCP23017.h>


const char *ssid = "SternenhimmelAP";
const char *password = "SternenhimmelAP";

ESP8266WebServer server(80);

/*
MCP23017 mcp1 = MCP23017(0x20);
MCP23017 mcp2 = MCP23017(0x21);
MCP23017 mcp3 = MCP23017(0x22);
MCP23017 mcp4 = MCP23017(0x23);
MCP23017 mcp5 = MCP23017(0x24);
MCP23017 mcp6 = MCP23017(0x25);
MCP23017 mcp7 = MCP23017(0x26);
MCP23017 mcp8 = MCP23017(0x27);
*/

MCP23017 mcp_objects[8] = {
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

void handleAPI() {
  Serial.println("-----handleAPI request-----");
  Serial.println("Request arguments: ");
  int i;
  for (i=0; i<server.args(); i++) {
    Serial.println(" - " + server.argName(i) + " = " + server.arg(i));
  }

  if((server.args() != 1) || (server.argName(0) != "cmd")) {
    server.send(400, "text/html", "<h1>Error: Invalid or missing argument.</h1>");
    return;
  }

  /* Forward argument to weigl control */
  Serial1.print(server.arg(0));
  server.send(200, "text/html", "<h1>Ok.</h1>");
}

void setup() {
  delay(1000);
  Wire.begin(0,2);
  //Wire.beginTransmission(I2CAddressESPWifi);
  //Wire.write(x);
  //Wire.endTransmission();
  //Wire.requestFrom(I2CAddressESPWifi,10);
  //Serial.print(“Request Return:[“);
  //while (Wire.available())
  //{
  //delay(1);
  //char c = Wire.read();
  //Serial.print(c);
  //}
  
  Serial.begin(9600);
  Serial.println("Configuring access point...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/", handleRoot);
  server.on("/api", handleAPI);
  server.begin();
  
  Serial.println("HTTP server started.");
}

void loop() {
  server.handleClient();
}
