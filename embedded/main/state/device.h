#ifndef DEVICE_H
#define DEVICE_H

#include "esp_err.h"

esp_err_t initialize_device_state(void);

char *get_device_id(void);

#endif
