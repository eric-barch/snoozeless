#ifndef DEVICE_H
#define DEVICE_H

#include "CurrentTime/CurrentTime.h"
#include "Display.h"
#include "NvsManager.h"
#include "Session.h"
#include "freertos/idf_additions.h"

class Device {
public:
  Device(NvsManager &nvs_manager, Session &session, CurrentTime &current_time,
         Display &display);
  ~Device();

  void subscribe();

private:
  NvsManager &nvs_manager;
  Session &session;
  CurrentTime &current_time;
  Display &display;
  std::string id;
  SemaphoreHandle_t is_blocked;

  void set_id(std::string);

  static void enroll_on_data(void *device, const std::string &response);
  void enroll();
  static void subscribe_on_data(void *device, const std::string &response);
  static void subscribe_task(void *pvParameters);
};

#endif // DEVICE_H
