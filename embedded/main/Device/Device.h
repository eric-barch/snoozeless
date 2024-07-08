#ifndef DEVICE_H
#define DEVICE_H

#include "NvsManager.h"
#include "Session.h"

class Device {
public:
  Device(NvsManager &nvs_manager, Session &session);

  void subscribe();

private:
  NvsManager &nvs_manager;
  Session &session;
  std::string id;

  void init();
  static void enroll_on_data(void *device_context, const std::string &response);
  void enroll();
};

#endif // DEVICE_H
