#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "Alarms.h"
#include "Button.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "Device.h"
#include "Display.h"

enum State { DISPLAY_TIME, WAKE_UP, COUNT_DOWN, SOUND_FOREVER };

class StateMachine {
public:
  StateMachine(Device &device, Alarms &alarms, CurrentTime &current_time,
               Button &button, Display &display, Buzzer &buzzer);

  ~StateMachine();

private:
  static const char *const TAG;

  Device &device;
  Alarms &alarms;
  CurrentTime &current_time;
  Button &button;
  Display &display;
  Buzzer &buzzer;
  State state;

  static void run(void *const pvParameters);
};

#endif
