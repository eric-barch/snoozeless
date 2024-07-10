#include "CurrentTime.h"
#include "Device.h"
#include "Display.h"
#include "NvsManager.h"
#include "Session.h"
#include "WifiManager.h"
#include "freertos/idf_additions.h"

static const char *TAG = "main";

extern "C" void app_main(void) {
  NvsManager nvs_manager;

  // nvs_manager.erase_key("session", "auth_bearer");
  // nvs_manager.erase_key("session", "refresh");
  // nvs_manager.erase_key("device", "id");

  WifiManager wifi_manager(nvs_manager);
  wifi_manager.connect();

  // TODO:
  // 1. Refactor inits into constructors.
  // 2. Add logs to all destructors.
  Session session(nvs_manager);
  CurrentTime current_time(nvs_manager, session);
  Display display(nvs_manager, current_time);

  Device device(nvs_manager, session, current_time, display);

  // session.keep_active();
  // device.subscribe();
  // current_time.subscribe();

  display.count();

  while (true) {
    vTaskDelay(portMAX_DELAY);
  }
}
