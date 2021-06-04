#include <Arduino.h>

#include "network.h"
#include "httpsRequests.h"
#include "timing.h"

MasterClock masterClock = MasterClock();

Timer syncMasterClockWithServerTimer = Timer(300000);

void setup() {
  Serial.begin(115200);
  Serial.println("\n");
  
  connectToWifi();
  masterClock.syncWithServer();
}

void loop() {
  if (syncMasterClockWithServerTimer.hasElapsed()) {
    Serial.println("\nsyncMasterClockWithServerTimer elapsed.");
    masterClock.syncWithServer();
  } else {
    Serial.printf("Current time: %d\n", masterClock.getDisplayTime());
  }
  delay(5000);
}