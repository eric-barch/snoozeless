#include <Arduino.h>

#include "appState.h"
#include "network.h"
#include "httpsRequests.h"
#include "timing.h"
#include "display.h"

Timer syncMasterClockWithServerTimer = Timer(300000);
Timer syncAppStateWithServerTimer = Timer(10000);
Timer renderDisplayTimer = Timer(5000);

void setup() {
  Serial.begin(115200);
  Serial.print("\n\n");
  
  network.connect();
  display.begin();
  
  appState.userSettings.timeZoneDifference = -14400;
  appState.userSettings.display.brightness = 1;
  appState.userSettings.display.militaryTime = false;

  masterClock.syncWithServer();
  appState.downloadFromServer();
  display.render(masterClock.getDisplayTime());
}

void loop() {
  if (syncMasterClockWithServerTimer.hasElapsed()) {
    Serial.print("\nsyncMasterClockWithServerTimer elapsed.\n");
    masterClock.syncWithServer();
  }

  if (syncAppStateWithServerTimer.hasElapsed()) {
    appState.downloadFromServer();
  }
  
  if (renderDisplayTimer.hasElapsed()) {
    display.render(masterClock.getDisplayTime());
  }

  delay(500);
}