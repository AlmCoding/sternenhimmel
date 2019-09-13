#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h> 
#include <Wire.h>


const char *ssid = "SternenhimmelAP";
const char *password = "SternenhimmelAP";

ESP8266WebServer server(80);

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
