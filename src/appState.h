struct UserSettings {
    // Time zone difference
    int timeZoneDifference;
    
    // Display variables
    int displayBrightness;
    bool displayMilitaryTime;

    // TODO: Add support for scheduling changes in display brightness. Server side?
};

UserSettings userSettings;

struct NextAlarm {
    // Activity
    bool armed;
    bool daysOfWeek[7];
    
    // Alarm 0 (Wakeup Alarm)
    int alarm0StartTime;
    bool alarm0Active;

    // Snoozes (Optional)
    int numberOfSnoozes;
    int snoozeDuration;

    // Countdown
    int countdownStartTime;
    bool countdownActive;
    int countdownDuration;

    // Destination
    bool destinationArrived;

    // Alarm 1 (Destination Alarm)
    int alarm1StartTime;
    bool alarm1Active;
};

NextAlarm nextAlarm;