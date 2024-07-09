#ifndef DEVICE_H
#define DEVICE_H

#include "CurrentTime/CurrentTime.h"
#include "NvsManager.h"
#include "Session.h"

class Device {
public:
  Device(NvsManager &nvs_manager, Session &session, CurrentTime &current_time);

private:
  NvsManager &nvs_manager;
  Session &session;
  CurrentTime &current_time;
  std::string id;

  static void enroll_on_data(void *device_instance,
                             const std::string &response);
  void enroll();
  void init();
};

#endif // DEVICE_H
