class MasterClock {
  
  static const int SECONDS_PER_DAY = 86400;
  static const int SECONDS_PER_HOUR = 3600;
  static const int SECONDS_PER_MINUTE = 60;

  static unsigned long serverUnixAtLastSync;
  static unsigned long localSecondsAtLastSync;

  public:
    // Sync MasterClock unix time with server.
    static void syncWithServer() {
      unsigned long serverUnix = getServerUnix();
      if (serverUnix != -1) {
        serverUnixAtLastSync = serverUnix;
        localSecondsAtLastSync = millis() / 1000;
      }
      Serial.printf("\nserverUnixAtLastSync: %lu\n", serverUnixAtLastSync);
      Serial.printf("localSecondsAtLastSync: %lu\n\n", localSecondsAtLastSync);
    }

    // Get current unix time.
    static unsigned long getUnixTime() {
      int currentLocalSeconds = millis() / 1000;
      int secondsSinceLastSync = currentLocalSeconds - localSecondsAtLastSync;
      return serverUnixAtLastSync + secondsSinceLastSync;
    }

    // Get current time in display format (HHMM or MMSS depending on mode).
    static int getDisplayTime() {
      return (hours() * 100) + minutes();
    }
  
  private:
    // Get seconds elapsed since midnight today.
    static int secondsElapsedToday() {
      return getUnixTime() % SECONDS_PER_DAY;
    }

    // Get hours place of current time.
    static int hours() {
      return secondsElapsedToday() / SECONDS_PER_HOUR;
    }

    // Get minutes place of current time.
    static int minutes() {
      return (secondsElapsedToday() % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
    }

    // Get seconds place of current time.
    static int seconds() {
      return secondsElapsedToday() % SECONDS_PER_MINUTE;
    }

};

unsigned long MasterClock::serverUnixAtLastSync = 0;
unsigned long MasterClock::localSecondsAtLastSync = 0;

class Timer {

  unsigned long millisAtLastUpdate;
  unsigned long updateInterval;

  public:
    // Constructor resets millisAtLastUpdate to current local millis 
    // and sets updateInterval.
    Timer(unsigned long ui) {
      setMillisAtLastUpdate();
      setUpdateInterval(ui);
    }

    // Set millisAtLastUpdate to current local millis.
    void setMillisAtLastUpdate() {
      millisAtLastUpdate = millis();
    }

    // Get local millis last time Timer was reset.
    unsigned long getMillisAtLastUpdate() {
      return millisAtLastUpdate;
    }

    // Set Timer update interval in millis.
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