struct AppState {

    struct UserSettings {    
        // Display variables
        struct Display {
            int brightness;
            bool militaryTime;
        };
        Display display;

        // Time zone difference
        int timeZoneDifference;

        // TODO: Add support for scheduling changes in display brightness. Server side operation?
    };
    UserSettings userSettings;

    struct NextAlarm {
        // Activation
        struct Activation {
            bool armed;
            bool daysOfWeek[7];
        };
        Activation activation;
        
        // Wakeup Alarm
        struct Alarm {
            bool active;
            int startTime;
        };
        Alarm alarm;

        // Alarm snoozes
        struct AlarmSnooze {
            int duration;
            int numberAllowed;
            int numberOccurred;
        };
        AlarmSnooze alarmSnooze;

        // Countdown
        struct Countdown {
            bool active;
            int duration;
            int startTime;
        };
        Countdown countdown;

        struct CountdownElapsedAlarm {
            bool active;
            int startTime;
        };
        CountdownElapsedAlarm countdownElapsedAlarm;

        // Destination
        struct Destination {
            bool arrived;
        };
        Destination destination;
    };
    NextAlarm nextAlarm;

    void downloadFromServer() {

    }

};
AppState appState;