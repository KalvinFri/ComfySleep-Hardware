#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>
const char* ssid = "Kalvin";         // WiFi ssid
const char* password = "free wifi";  // WiFi Password
WiFiServer server(80);
int analogPin = A0;
int val = 0;
const int relay = 5;
bool on = false;
//****** Define the calibration parameters
const float analogMin = 0.0;     // Minimum analog reading from the FSR during calibration
const float analogMax = 1023.0;  // Maximum analog reading from the FSR during calibration
const float forceMin = 0.0;      // Minimum physical force applied during calibration (in some unit)
const float forceMax = 100;     // Maximum physical force applied during calibration (in the same unit)
void setup() {
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  Serial.println("Current not Flowing");
  while (!Serial)
    delay(10);  // will pause Zero, Leonardo, etc until serial console opens
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  };
  server.begin();
  Serial.println("Server started");
  /* Print the IP address on serial monitor */
  Serial.print("Use this URL to connect: ");
  Serial.print("http://"); /* URL IP to be typed in mobile/desktop browser */
  Serial.print(WiFi.localIP());
  Serial.println("/");
}
void loop() {
  // Normally Open configuration, send LOW signal to let current flow
  // (if you're usong Normally Closed configuration send HIGH signal)
  // Normally Open configuration, send HIGH signal stop current flow
  // (if you're usong Normally Closed configuration send LOW signal)
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  /* Wait until the client sends some data */
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }
  /* Read the first line of the request */
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  StaticJsonDocument<200> data;
  if (request.indexOf("/on") != -1) {
    digitalWrite(relay, LOW);
    Serial.println("Current Flowing");
    // delay(72500);
    // digitalWrite(relay, HIGH);
    data["status"] = "ok";
  }

  if (request.indexOf("/on50") != -1) {
    digitalWrite(relay, LOW);
    Serial.println("Current Flowing");
    delay(72500);
    digitalWrite(relay, HIGH);
    data["status"] = "ok";
  }

  if (request.indexOf("/on90") != -1) {
    digitalWrite(relay, LOW);
    Serial.println("Current Flowing");
    delay(130000);
    digitalWrite(relay, HIGH);
    data["status"] = "ok";
  }

  if (request.indexOf("/off") != -1) {
    digitalWrite(relay, HIGH);
    Serial.println("Current not Flowing");
    data["status"] = "ok";
  }
  // *******
  if (request.indexOf("/flex") != -1) {
    int fsrValue = analogRead(A0);
    // Apply the calibration mapping function
    float force = mapFloat(fsrValue, analogMin, analogMax, forceMin, forceMax);
    Serial.print("Raw Analog Reading: ");
    Serial.print(fsrValue);
    Serial.print(", Force: ");
    Serial.print(floor(force));
    Serial.println(" (units)");
    data["fsrValue"] = fsrValue;
    data["force"] = force;
  }
  String dataBody;
  serializeJson(data, dataBody);
  Serial.println(dataBody);
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json; charset=utf-8");
  client.println(""); /* do not forget this one */
  client.println(dataBody);
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
  delay(1000);
}
// ******** Custom map function for floating-point values
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}