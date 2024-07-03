#ifndef DEVICE_H
#define DEVICE_H

#include "NvsManager.h"
#include "Session.h"
#include "esp_err.h"

class Device {
private:
  NvsManager &nvs_manager;
  Session &session;
  std::string id;

  esp_err_t enroll();

public:
  Device(NvsManager &nvs_manager, Session &session);

  esp_err_t subscribe();
};

#endif // DEVICE_H
