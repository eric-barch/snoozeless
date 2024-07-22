#ifndef DEVICE_H
#define DEVICE_H

#include "Buzzer.h"
#include "CurrentTime.h"
#include "Display.h"
#include "NvsManager.h"
#include "Session.h"
#include <esp_err.h>
#include <freertos/idf_additions.h>

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
  std::string id;
  SemaphoreHandle_t is_subscribed;

  void set_id(std::string);

  static void enroll_on_data(void *device, const std::string &response);
  esp_err_t enroll();

  static void subscribe_on_data(void *device, const std::string &response);
  void subscribe();
  static void subscribe_task(void *pvParameters);
  void keep_subscribed();
};

#endif // DEVICE_H
