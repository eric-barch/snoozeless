#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "get_device_state.h"
#include "state/device_state.h"
#include <stdio.h>
#include <stdlib.h>

static const char *TAG = "services/subscribe_to_device";

void subscribe_to_device(void *pvParameters) {
  char *deviceId = get_device_id();
  if (deviceId == NULL) {
    ESP_LOGE(TAG, "Failed to get device ID.");
    vTaskDelete(NULL);
    return;
  }
  get_device_state(deviceId);
  vTaskDelete(NULL);
}
