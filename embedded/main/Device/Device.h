#ifndef DEVICE_H
#define DEVICE_H

#include "Alarms.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "Display.h"
#include "NonVolatileStorage.h"
#include "Session.h"
#include <esp_err.h>
#include <map>

enum DeviceEvent {
  INITIAL_DEVICE,
  DEVICE_UPDATE,
  INITIAL_ALARMS,
  ALARM_INSERT,
  ALARM_UPDATE,
  ALARM_DELETE,
};

class Device {
public:
  Device(NonVolatileStorage &non_volatile_storage, Session &session,
         CurrentTime &current_time, Alarms &alarms, Display &display,
         Buzzer &buzzer);

  ~Device();

  void on_data(const std::string &response);

private:
  static const char *const TAG;
  static const std::map<const std::string, const DeviceEvent> events;

  NonVolatileStorage &non_volatile_storage;
  Session &session;
  CurrentTime &current_time;
  Alarms &alarms;
  Display &display;
  Buzzer &buzzer;
  std::string id;

  void set_id(const std::string &id);

  static void handle_subscribe(void *const pvParameters);

  esp_err_t enroll();
  void subscribe();
  void parse(const std::string &device_string);
  void extract_sse_field(const std::string &response, const std::string &field,
                         std::string &out_value);
  void parse_sse(const std::string &response);
};

#endif // DEVICE_H
