#include "services/device.h"
#include "controllers/get_device_state.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "post_device_register.h"
#include "state/device.h"
#include <stdio.h>
#include <stdlib.h>

static const char *TAG = "services/device";

esp_err_t register_device(device_state_t *device) {
  esp_err_t err = post_device_register(device);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to post to device/register.");
  }
  return err;
}

void subscribe_to_device_state_task(void *pvParameters) {
  char *deviceId = get_device_id();
  if (deviceId == NULL) {
    ESP_LOGE(TAG, "Failed to get device ID.");
    vTaskDelete(NULL);
    return;
  }
  get_device_state(deviceId);
  vTaskDelete(NULL);
}
