#include "console_utils.h"
#include "freertos/idf_additions.h"
#include "http_requests.h"
#include "nvs_utils.h"
#include "wifi_utils.h"

void app_main(void) {
  initialize_nvs();
  initialize_console();

  initialize_wifi();

  close_nvs();
  close_console();

  xTaskCreate(&get_device_state, "get_device_state", 4096, NULL, 5, NULL);
}
