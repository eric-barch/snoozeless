#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include "NonVolatileStorage.h"
#include <esp_err.h>
#include <esp_event_base.h>
#include <freertos/idf_additions.h>
#include <string>

class WifiConnection {
public:
  WifiConnection(NonVolatileStorage &non_volatile_storage);

  ~WifiConnection();

private:
  static const char *const TAG;
  static const int WIFI_CONNECTED_BIT;
  static const int WIFI_FAIL_BIT;
  static const int MAX_RETRY;

  NonVolatileStorage &non_volatile_storage;
  std::string ssid;
  std::string password;
  EventGroupHandle_t wifi_event_group;
  int retry_count;

  void set_ssid(const std::string &ssid);
  void set_password(const std::string &password);

  static void handle_wifi_event(void *const arg, esp_event_base_t event_base,
                                int32_t event_id, void *const event_data);

  esp_err_t connect();
  esp_err_t disconnect();
};

#endif // WIFI_CONNECTION_H
