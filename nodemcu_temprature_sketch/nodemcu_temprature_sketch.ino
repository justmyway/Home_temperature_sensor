//#include <iostream>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include "version.h"
#include "config.h"
#include "device.h"
 
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* version = VERSION;

const char* type = TYPE;

WiFiServer server(80);
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Reachable on: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  Serial.println("");
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    delay(10);
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
  if (request.indexOf("/api") != -1)  {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.println("<!DOCTYPE HTML>");
    client.println("<html lang=\"en\">");
    client.println("<head>");
    client.println("<meta charset=\"utf-8\">");
    client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" rel=\"stylesheet\">");
    client.println("</head>");
    client.println("<body>");
    client.println("<div class=\"container\">");
    
    client.print("<h1>Runing api version: ");
    client.print(version);
    client.print("</h1>");

    client.print("<ul>");
    client.print("<li>GET `/device` returns the device type</li>");
    client.print("<li>GET `/value` returns all sensor values</li>");
    client.print("</ul>");

    client.print("<p>This code was created by Michael van de Ven @justmyway industries.</p>");

    client.println("</div>");
    client.println("</body>"); 
    client.println("</html>");   
  }

  if (request.indexOf("/device") != -1)  {
    Serial.println("device");
    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root["type"] = type;
    root["active"] = true;

    if (!root.success()) {
      client.println("HTTP/1.1 500 ERROR");
      client.println("");
      return;
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Connection: close\r\nContent-Type: application/json");
    client.println(""); //  do not forget this one
    root.printTo(client);
  }
  
  if (request.indexOf("/value") != -1)  {
    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root["device"] = type;
    JsonArray& values = root.createNestedArray("values");
    
    JsonObject& value = values.createNestedObject();
    value["temperature"] = 14.2;
    
    root["request_at"] = "right now";

    if (!root.success()) {
      client.println("HTTP/1.1 500 ERROR");
      client.println("");
      return;
    }
    
    client.println("HTTP/1.1 200 OK");
    client.println("Connection: close\r\nContent-Type: application/json");
    client.println(""); //  do not forget this one
    root.printTo(client);
  }
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
