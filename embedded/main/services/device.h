#ifndef SERVICES_DEVICE_H
#define SERVICES_DEVICE_H

#include "state/device.h"

esp_err_t register_device(device_state_t *device);

void subscribe_to_device_state(void *pvParameters);

#endif
