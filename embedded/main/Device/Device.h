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

enum DeviceStateEvent {
  INITIAL_DEVICE,
  DEVICE_UPDATE,
  INITIAL_ALARMS,
  ALARM_INSERT,
  ALARM_UPDATE,
  ALARM_DELETE,
  UNKNOWN_EVENT
};

class Device {
public:
  Device(NonVolatileStorage &non_volatile_storage, Session &session,
         CurrentTime &current_time, Alarms &alarms, Display &display,
         Buzzer &buzzer);

  void on_data(const std::string &response);

private:
  NonVolatileStorage &non_volatile_storage;
  Session &session;
  CurrentTime &current_time;
  Alarms &alarms;
  Display &display;
  Buzzer &buzzer;
  std::string id;
  SemaphoreHandle_t is_subscribed;

  void set_id(std::string &id);

  static void subscribe_task(void *pvParameters);

  void parse_device_state(const std::string &data);
  esp_err_t enroll();
  void subscribe();
  void keep_subscribed();
};

#endif // DEVICE_H
