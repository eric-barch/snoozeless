#ifndef DEVICE_H
#define DEVICE_H

#include "Alarm.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "Display.h"
#include "NvsManager.h"
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
  ALARM_DELETE,
  ALARM_UPDATE,
  UNKNOWN_EVENT
};

class Device {
public:
  Device(NvsManager &nvs_manager, Session &session, CurrentTime &current_time,
         Display &display, Buzzer &buzzer);
  ~Device();

private:
  NvsManager &nvs_manager;
  Session &session;
  CurrentTime &current_time;
  Display &display;
  Buzzer &buzzer;
  std::vector<Alarm *> alarms;
  std::string id;
  SemaphoreHandle_t is_subscribed;

  void set_id(std::string);

  void initialize_alarms();
  static void enroll_on_data(void *device, const std::string &response);
  esp_err_t enroll();

  static void parse_device(void *device, const std::string &data);
  static void parse_initial_alarms(void *device, const std::string &data);
  static void parse_alarm_insert(void *device, const std::string &data);
  static void parse_alarm_delete(void *device, const std::string &data);
  static void parse_alarm_update(void *device, const std::string &data);

  static void subscribe_on_data(void *device, const std::string &response);
  void subscribe();
  static void subscribe_task(void *pvParameters);
  void keep_subscribed();
};

#endif // DEVICE_H
