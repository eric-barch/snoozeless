#include <Arduino.h>

#include "appState.h"
#include "network.h"
#include "httpsRequests.h"
#include "timing.h"
#include "display.h"

Timer syncMasterClockWithServerTimer = Timer(300000);
Timer renderDisplayTimer = Timer(5000);

void setup() {
  Serial.begin(115200);
  Serial.print("\n\n");
  
  network.connect();
  display.begin();
  
  userSettings.timeZoneDifference = -14400;
  userSettings.displayBrightness = 1;
  userSettings.displayMilitaryTime = false;

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