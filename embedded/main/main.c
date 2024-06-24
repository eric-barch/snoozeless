#include "freertos/idf_additions.h"
#include "services/device.h"
#include "services/real_time.h"
#include "state/app_credentials.h"
#include "state/device.h"
#include "state/wifi_credentials.h"
#include "utilities/console.h"
#include "utilities/nvs.h"
#include "utilities/wifi.h"

void app_main(void) {
  initialize_nvs();
  initialize_console();

  initialize_wifi_credentials();
  connect_to_wifi();

  initialize_app_credentials();
  initialize_device_state();

  close_nvs();
  close_console();

  xTaskCreate(&calibrate_real_time, "calibrate_real_time", 4096, NULL, 5, NULL);
  xTaskCreate(&subscribe_to_device_state, "subscribe_to_device_state", 4096,
              NULL, 5, NULL);
}
