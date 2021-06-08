#include <WiFi.h>

class Network {

  static const char* ssid;
  static const char* password;

  public:
    static void connect() {
      WiFi.begin(ssid, password);
      
      Serial.print("Connecting to the WiFi network...");
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.print("\nConnected to the WiFi network.\n");
    }

    static void reconnect() {
      Serial.print("Reconnecting to the WiFi network...\n");
      bool reconnectionSuccessful = WiFi.reconnect();
      Serial.print(reconnectionSuccessful ? "Reconnected.\n" : "Reconnection failed.\n");
    }
};

// TODO: Add support for multiple WiFi network memory.
const char* Network::ssid = "ORBI39";
const char* Network::password = "Percy693#";