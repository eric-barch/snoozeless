#include <WiFi.h>

const char* ssid = "ORBI39";
const char* password = "Percy693#";

void connectToWifi() {
  
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to the WiFi network...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("\nConnected to the WiFi network.\n");

}