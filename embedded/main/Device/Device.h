#ifndef DEVICE_H
#define DEVICE_H

#include "Alarms.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "Display.h"
#include "NonVolatileStorage.h"
#include "Session.h"
#include <esp_err.h>

class Device {
public:
  Device(NonVolatileStorage &non_volatile_storage, Session &session,
         CurrentTime &current_time, Alarms &alarms, Display &display,
         Buzzer &buzzer);

  ~Device();

  std::string get_id();

  void on_data(const std::string &response);
  void parse(const std::string &data);

private:
  static const char *const TAG;

  NonVolatileStorage &non_volatile_storage;
  Session &session;
  CurrentTime &current_time;
  Alarms &alarms;
  Display &display;
  Buzzer &buzzer;
  std::string id;

  void set_id(const std::string &id);

  esp_err_t enroll();
};

#endif // DEVICE_H
