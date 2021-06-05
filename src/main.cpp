#include <Arduino.h>

#include "network.h"
#include "httpsRequests.h"
#include "timing.h"
#include "display.h"

MasterClock masterClock = MasterClock();
Display display = Display();

Timer syncMasterClockWithServerTimer = Timer(300000);
Timer renderDisplayTimer = Timer(5000);

void setup() {
  Serial.begin(115200);
  Serial.println("\n");
  
  Network::connect();
  masterClock.syncWithServer();
  display.render(masterClock.getDisplayTime());
}

void loop() {
  if (syncMasterClockWithServerTimer.hasElapsed()) {
    Serial.printf("\nsyncMasterClockWithServerTimer elapsed.\n");
    masterClock.syncWithServer();
  } 
  
  if (renderDisplayTimer.hasElapsed()) {
    display.render(masterClock.getDisplayTime());
  }

  delay(500);
}