#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "NvsManager.h"
#include "esp_err.h"
#include "esp_event_base.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <string>

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define MAX_RETRY 5

class WifiManager {
public:
  WifiManager(NvsManager &nvs_manager);
  ~WifiManager();

  esp_err_t connect();
  esp_err_t disconnect();

private:
  NvsManager &nvs_manager;
  EventGroupHandle_t wifi_event_group;
  int retry_count;
  std::string ssid;
  std::string password;

  void set_ssid(std::string ssid);
  void set_password(std::string password);

  static void handle_wifi_event(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data);
};

#endif // WIFI_MANAGER_H
