#include "freertos/idf_additions.h"
#include "services/subscribe_to_device.h"
#include "state/app_credentials.h"
#include "state/device_state.h"
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

  xTaskCreate(&subscribe_to_device, "subscribe_to_device", 4096, NULL, 5, NULL);
}
