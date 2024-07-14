#ifndef DEVICE_H
#define DEVICE_H

#include "CurrentTime/CurrentTime.h"
#include "Display.h"
#include "NvsManager.h"
#include "Session.h"

class Device {
public:
  Device(NvsManager &nvs_manager, Session &session, CurrentTime &current_time,
         Display &display);
  ~Device();

private:
  NvsManager &nvs_manager;
  Session &session;
  CurrentTime &current_time;
  Display &display;
  std::string id;

  void set_id(std::string);

  static void enroll_on_data(void *device, const std::string &response);
  void enroll();
};

#endif // DEVICE_H
