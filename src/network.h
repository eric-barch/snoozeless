#include <WiFi.h>

class Network {

  const char* ssid = "ORBI92";
  const char* password = "deepbug350";

  public:
    // Connect to WiFi network for the first time.
    void connect() {
      WiFi.begin(ssid, password);
      
      Serial.print("Connecting to the WiFi network...");
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.print("\nConnected to the WiFi network.\n");
    }

    // Reconnect to the WiFi network after connection is lost.
    void reconnect() {
      Serial.print("Reconnecting to the WiFi network...\n");
      bool reconnectionSuccessful = WiFi.reconnect();
      Serial.print(reconnectionSuccessful ? "Reconnected.\n" : "Reconnection failed.\n");
    }
};

Network network;