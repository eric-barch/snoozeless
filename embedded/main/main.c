#include "freertos/idf_additions.h"
#include "http_requests.h"
#include "nvs_utils.h"
#include "p_read_device_stream.h"
#include "s_app_credentials.h"
#include "s_device.h"
#include "s_wifi_credentials.h"
#include "wifi_utils.h"

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
