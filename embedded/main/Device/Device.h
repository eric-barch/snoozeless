#ifndef DEVICE_H
#define DEVICE_H

#include "Alarms.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "Display.h"
#include "NonVolatileStorage.h"
#include "Session.h"
#include <esp_err.h>
#include <freertos/idf_additions.h>
#include <map>
#include <vector>

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
  SemaphoreHandle_t is_subscribed;

  void set_id(std::string &id);

  esp_err_t extract_response_field(const std::string &response,
                                   const std::string &field,
                                   std::string &out_value);
  void parse(const std::string &data);

  esp_err_t enroll();
  void subscribe();
  static void keep_subscribed(void *pvParameters);
};

#endif // DEVICE_H
