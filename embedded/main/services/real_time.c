#include "services/real_time.h"
#include "controllers/get_real_time.h"
#include "freertos/idf_additions.h"

void calibrate_real_time(void *pvParameters) {
  while (true) {
    get_real_time();
    vTaskDelay(pdMS_TO_TICKS(3600000));
  }
}
