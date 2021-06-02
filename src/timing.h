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