#include <WiFi.h>

class Network {

  static const char* ssid;
  static const char* password;

  public:
    // TODO: Add timeout.
    static void connect() {
      WiFi.begin(ssid, password);
      
      Serial.print("Connecting to the WiFi network...");
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.print("\nConnected to the WiFi network.\n");
    }

    // TODO: Add function with timeout for reconnecting to WiFi.
    static void reconnect() {
      Serial.print("Reconnecting to the WiFi network...\n");
      bool reconnectionSuccessful = WiFi.reconnect();
      Serial.print(reconnectionSuccessful ? "Reconnected.\n" : "Reconnection failed.\n");
    }
};

const char* Network::ssid = "ORBI39";
const char* Network::password = "Percy693#";