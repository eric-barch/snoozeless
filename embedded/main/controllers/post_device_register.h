#ifndef CONTROLLERS_POST_DEVICE_REGISTER_H
#define CONTROLLERS_POST_DEVICE_REGISTER_H

#include "esp_err.h"
#include "state/device.h"

esp_err_t post_device_register(device_state_t *device);

#endif
