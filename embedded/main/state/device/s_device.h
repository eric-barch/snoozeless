#ifndef S_DEVICE_H
#define S_DEVICE_H

#include "esp_err.h"

esp_err_t initialize_device_state(void);

const char *get_device_id(void);

#endif
