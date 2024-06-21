#include "controllers/post_device_register.h"
#include "esp_err.h"
#include "state/device_state.h"

esp_err_t register_device(device_t *device) {
  esp_err_t err = post_device_register(device);
  return err;
}
