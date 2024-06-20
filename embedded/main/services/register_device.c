#include "services/register_device.h"
#include "controllers/post_device_register.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"

static const char *TAG = "services/register_device";

esp_err_t register_device(void *pvParameters) {
  esp_err_t err = post_device_register();
  vTaskDelete(NULL);
  return err;
}
