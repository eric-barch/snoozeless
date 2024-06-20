#include "freertos/idf_additions.h"
#include "services/read_device_stream.h"
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
  initialize_app_credentials();
  initialize_device_state();

  close_nvs();
  close_console();

  connect_to_wifi();

  xTaskCreate(&read_device_stream, "read_device_stream", 4096, NULL, 5, NULL);
}
