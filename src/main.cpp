#include <Arduino.h>

#include "network.h"
#include "httpsRequests.h"
#include "timing.h"
#include "display.h"

Timer syncMasterClockWithServerTimer = Timer(300000);
Timer renderDisplayTimer = Timer(5000);

void setup() {
  Serial.begin(115200);
  Serial.println("\n");
  
  Network::connect();
  Display::begin();

  MasterClock::syncWithServer();
  Display::render(MasterClock::getDisplayTime());
}

void loop() {
  if (syncMasterClockWithServerTimer.hasElapsed()) {
    Serial.printf("\nsyncMasterClockWithServerTimer elapsed.\n");
    MasterClock::syncWithServer();
  } 
  
  if (renderDisplayTimer.hasElapsed()) {
    Display::render(MasterClock::getDisplayTime());
  }

  delay(500);
}