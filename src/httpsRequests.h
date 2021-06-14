#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* timeApi = "https://us-central1-runtimealarm-f8654.cloudfunctions.net/user/OL8UIxPo8dPG1OoGGIK5280SW4H2/time";
const char* appStateApi = "foo";

// TODO: Make generalized HTTPS request function and feed various APIs into it.

// Get server unix time.
unsigned long getServerUnix() {
  
  unsigned long serverUnix;
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi. Attempting to reconnect.\n");
    network.reconnect();
  }

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.begin(timeApi, nullptr);
    http.useHTTP10(true);
    http.setTimeout(10000);

    int httpCode = http.GET();

    if (httpCode > 0) {

      if (httpCode == HTTP_CODE_OK) {
        // Get a reference to the stream in HTTPClient.
        Stream& response = http.getStream();

        // Create JsonDocument in the stack.
        StaticJsonDocument<64> doc;

        // Deserialize the JSON document in the response.
        DeserializationError err = deserializeJson(doc, response);

        if (err) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(err.c_str());
          serverUnix = -1;
        } else {
          serverUnix = doc["unixTime"];
          Serial.println("HTTPS request successful.");
        }
      } else {
        Serial.print(F("HTTPS request failed with code "));
        Serial.println(httpCode);
        serverUnix = -1;
      }

    } else {
      Serial.println("HTTPS request failed internally with code ");
      Serial.println(httpCode);
      serverUnix = -1;
    }

    http.end();

  } else {
    Serial.println("HTTPS request failed due to lack of internet connection.");
    serverUnix = -1;
  }

  return serverUnix;

}

// Get any changed appState values from the server.
void getServerAppState() {
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi. Attempting to reconnect.\n");
    network.reconnect();
  }

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.begin(appStateApi, nullptr);
    http.useHTTP10(true);
    http.setTimeout(10000);

    int httpCode = http.GET();

    if (httpCode > 0) {

      if (httpCode == HTTP_CODE_OK) {
        // Get a reference to the stream in HTTPClient.
        Stream& response = http.getStream();

        // Create JsonDocument in the stack.
        StaticJsonDocument<128> doc;

        // Deserialize the JSON document in the response.
        DeserializationError err = deserializeJson(doc, response);

        if (err) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(err.c_str());
        } else {
          Serial.print("HTTPS request successful.\n");
          // ADD: Algorithm that maps JsonDocument to internal appState struct.
        }
      } else {
        Serial.print("HTTPS request failed with code ");
        Serial.println(httpCode);
      }

    } else {
      Serial.print("HTTPS request failed internall with code ");
      Serial.println(httpCode);
    }

    http.end();
  
  } else {
    Serial.print("HTTPS request failed due to lack of internet connection.\n");
  }

}