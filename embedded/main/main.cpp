#include "NvsManager/NvsManager.h"
#include "esp_log.h"
#include <string>

const char *TAG = "main";

extern "C" void app_main(void) {
  NvsManager nvs_manager;

  std::string wifi_ssid;

  nvs_manager.write_string("wifi_cred", "ssid", "ORBI39");
  nvs_manager.read_string("wifi_cred", "ssid", wifi_ssid);

  ESP_LOGI(TAG, "wifi_ssid: %s", wifi_ssid.c_str());

  // WifiManager wifi_manager(nvs_manager);
  // User user(nvs_manager);
  // Device device(nvs_manager, user);
  //
  // CurrentTime current_time(nvs_manager, user, device);
  // Display display(nvs_manager, current_time);
  //
  // user.maintain_session();
  // device.subscribe();
  // current_time.subscribe();
  //
  // display.show_current_time();
}
