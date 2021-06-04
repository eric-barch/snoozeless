#include <Arduino.h>

#include "internet.h"
#include "timing.h"

Timer syncUnixWithServerTimer = Timer(600000);

void setup() {
  Serial.begin(115200);
  Serial.println("\n");
  
  connectToWifi();
  syncUnixWithServer();
}

void loop() {
  if (syncUnixWithServerTimer.hasElapsed()) {
    Serial.println("syncUnixWithServerTimer elapsed.");
    syncUnixWithServer();
  } else {
    Serial.println("syncUnixWithServerTimer not elapsed.");
  }
  delay(5000);
}