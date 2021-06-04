class MasterClock {
  
  const int SECONDS_PER_DAY = 86400;
  const int SECONDS_PER_HOUR = 3600;
  const int SECONDS_PER_MINUTE = 60;

  unsigned long serverUnixAtLastSync;
  unsigned long localSecondsAtLastSync;

  public:
    // Sync unix clock with server.
    void syncWithServer() {
      unsigned long serverUnix = getServerUnix();
      if (serverUnix != -1) {
        serverUnixAtLastSync = serverUnix;
        localSecondsAtLastSync = millis() / 1000;
      }
      Serial.printf("\nserverUnixAtLastSync: %lu\n", serverUnixAtLastSync);
      Serial.printf("localSecondsAtLastSync: %lu\n\n", localSecondsAtLastSync);
    }

    // Return current unix time.
    unsigned long getUnixTime() {
      int currentLocalSeconds = millis() / 1000;
      int secondsSinceLastSync = currentLocalSeconds - localSecondsAtLastSync;
      return serverUnixAtLastSync + secondsSinceLastSync;
    }

    // Return current time in display format (HHMM or MMSS depending on mode).
    int getDisplayTime() {
      return (hours() * 100) + minutes();
    }
  
  private:
    // Return seconds elapsed since midnight today.
    int secondsElapsedToday() {
      return getUnixTime() % SECONDS_PER_DAY;
    }

    // Return hours place of current time.
    int hours() {
      return secondsElapsedToday() / SECONDS_PER_HOUR;
    }

    // Return minutes place of current time.
    int minutes() {
      return (secondsElapsedToday() % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
    }

    // Return seconds place of current time.
    int seconds() {
      return secondsElapsedToday() % SECONDS_PER_MINUTE;
    }

};

class Timer {

  unsigned long millisAtLastUpdate;
  unsigned long updateInterval;

  public:
    // Constructor resets millisAtLastUpdate to current clock 
    // and sets updateInterval.
    Timer(unsigned long ui) {
      setMillisAtLastUpdate();
      setUpdateInterval(ui);
    }

    // Set millisAtLastUpdate to current time.
    void setMillisAtLastUpdate() {
      millisAtLastUpdate = millis();
    }

    // Get millisAtLastUpdate.
    unsigned long getMillisAtLastUpdate() {
      return millisAtLastUpdate;
    }

    // Set updateInterval.
    void setUpdateInterval(unsigned long ui) {
      updateInterval = ui;
    }

    // Get updateInterval.
    unsigned long getUpdateInterval() {
      return updateInterval;
    }

    // Get milliseconds elapsed since timer last elapsed.
    unsigned long elapsedSinceLastUpdate() {
      return millis() - millisAtLastUpdate;
    }

    // Get whether timer has exceeded updateInterval.
    bool hasElapsed() {
      if (elapsedSinceLastUpdate() > updateInterval) {
        setMillisAtLastUpdate();
        return true;
      } else {
        return false;
      }
    }

};