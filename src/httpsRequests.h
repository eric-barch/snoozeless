#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* timeApi = "https://us-central1-runtimealarm-f8654.cloudfunctions.net/user/OL8UIxPo8dPG1OoGGIK5280SW4H2/time";

// Return server unix time.
unsigned long getServerUnix() {
  
  unsigned long serverUnix;

  if ((WiFi.status() == WL_CONNECTED)) {

    HTTPClient http;
    /* Passing nullptr as second argument to http.begin() disables SSL 
     * certificate verification. Improves maintainability after hardware has
     * shipped, but ensure API is trusted. */
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
          Serial.print(F("deserializeJson() failed with code "));
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
    /* TODO: Have the below line call the Network static function instead.
     * Also consider moving it to around line 12 so it can still make the
     * HTTPS request if it is able to reconnect. */
    Serial.printf("Reconnected to WiFi: %d\n", WiFi.reconnect());
    serverUnix = -1;
  }

  return serverUnix;

}