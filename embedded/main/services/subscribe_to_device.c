#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "state/device.h"
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
  vTaskDelete(NULL);
}
