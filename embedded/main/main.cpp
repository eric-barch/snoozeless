#include "Buzzer.h"
#include "CurrentTime.h"
#include "Device.h"
#include "Display.h"
#include "NvsManager.h"
#include "Session.h"
#include "WifiManager.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"

static const char *TAG = "main";

extern "C" void app_main(void) {
  NvsManager nvs_manager;

  WifiManager wifi_manager(nvs_manager);
  Session session(nvs_manager);
  CurrentTime current_time(nvs_manager, session);
  Display display(nvs_manager, current_time);
  Buzzer buzzer;
  Device device(nvs_manager, session, current_time, display);

  display.print_current_time();

  buzzer.start_buzzing();
  vTaskDelay(pdMS_TO_TICKS(30000));
  buzzer.stop_buzzing();

  while (true) {
    vTaskDelay(portMAX_DELAY);
  }
}
