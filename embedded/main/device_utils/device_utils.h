#ifndef DEVICE_UTILS_H
#define DEVICE_UTILS_H

#include "esp_err.h"

esp_err_t initialize_device_state(void);

const char *get_device_id(void);

#endif
