#ifndef DEVICE_H
#define DEVICE_H

#include "Alarms.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "Display.h"
#include "NonVolatileStorage.h"
#include "Session.h"
#include <esp_err.h>

enum DeviceState {
  DISPLAY_TIME,
  ALARM_1,
  COUNTDOWN,
  ALARM_2,
};

enum DeviceEvent {
  START_ALARM,
  PRESS_BUTTON,
  END_COUNTDOWN,
  ABORT,
};

class Device {
public:
  Device(NonVolatileStorage &non_volatile_storage, Session &session,
         CurrentTime &current_time, Alarms &alarms, Display &display,
         Buzzer &buzzer);

  ~Device();

  void set_state(const DeviceState &state);

  std::string get_id();
  DeviceState get_state();
  TaskHandle_t get_run_handle();

  void on_data(const std::string &response);
  void parse(const std::string &data);
  void display_current_time();
  void start_alarm();
  void stop_alarm();
  void display_countdown();

private:
  static const char *const TAG;
  static const std::map<const DeviceState, const std::string> states;
  static const std::map<const DeviceEvent, const std::string> events;

  NonVolatileStorage &non_volatile_storage;
  Session &session;
  CurrentTime &current_time;
  Alarms &alarms;
  Display &display;
  Buzzer &buzzer;
  std::string id;
  DeviceState state;
  TaskHandle_t run_handle;

  void set_id(const std::string &id);

  static void run(void *const pvParameters);

  esp_err_t enroll();
};

#endif // DEVICE_H
