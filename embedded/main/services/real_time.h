#ifndef SERVICES_REAL_TIME_H
#define SERVICES_REAL_TIME_H

#include "esp_err.h"

esp_err_t calibrate_real_time();

void calibrate_real_time_task(void *pvParameters);

#endif
