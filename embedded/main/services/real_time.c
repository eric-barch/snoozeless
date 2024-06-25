#include "services/real_time.h"
#include "controllers/get_unix_time.h"
#include "freertos/idf_additions.h"

void calibrate_real_time(void *pvParameters) {
  while (true) {
    get_unix_time();
    vTaskDelay(pdMS_TO_TICKS(15000));
  }
}
