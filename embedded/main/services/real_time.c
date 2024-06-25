#include "services/real_time.h"
#include "controllers/get_unix_time.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"

esp_err_t calibrate_real_time() {
  esp_err_t err = get_unix_time();
  return err;
}

void calibrate_real_time_task(void *pvParameters) {
  while (true) {
    calibrate_real_time();
    /**Recalibrate every 24 hours (24 * 60 * 60 * 1000). */
    vTaskDelay(pdMS_TO_TICKS(86400000));
  }
}
