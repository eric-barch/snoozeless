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

  WifiManager wifi_manager(nvs_manager);
  Session session(nvs_manager);
  CurrentTime current_time(nvs_manager, session);
  Display display(nvs_manager, current_time);
  Device device(nvs_manager, session, current_time, display);

  display.print_current_time();

  while (true) {
    vTaskDelay(portMAX_DELAY);
  }
}

// TODO:
// 1. Make sure threaded tasks are allocated least amount of stack memory
// possible.
// 2. Refactor semaphores to direct-to-task notifications.
