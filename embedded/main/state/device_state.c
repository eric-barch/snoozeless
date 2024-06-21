#include "state/device_state.h"
#include "esp_err.h"
#include "services/device.h"
#include "utilities/nvs.h"
#include <stdbool.h>
#include <string.h>

device_t device;

esp_err_t initialize_device_state(void) {
  esp_err_t err = get_nvs_str("device", "id", device.id, MAX_ID_LENGTH);
  if (err != ESP_OK) {
    err = register_device(&device);
    if (err != ESP_OK) {
      return err;
    }

    err = set_device_id(device.id);
    if (err != ESP_OK) {
      return err;
    }
  }

  err = get_nvs_str("device", "time_format", device.time_format,
                    MAX_TIME_FORMAT_LENGTH);
  if (err != ESP_OK) {
    err = set_device_time_format(device.time_format);
  }

  return err;
};

esp_err_t set_device_id(const char *id) {
  strncpy(device.id, id, MAX_ID_LENGTH);
  device.id[MAX_ID_LENGTH - 1] = '\0';

  esp_err_t err = set_nvs_str("device", "id", id);
  return err;
}

char *get_device_id(void) { return device.id; }

static bool is_valid_time_format(const char *time_format) {
  return (strcmp(time_format, "HH:MM") == 0 ||
          strcmp(time_format, "HH:MM XM") == 0);
}

esp_err_t set_device_time_format(char *time_format) {
  if (!is_valid_time_format(time_format)) {
    return ESP_FAIL;
  }

  strncpy(device.time_format, time_format, MAX_TIME_FORMAT_LENGTH - 1);
  device.time_format[MAX_TIME_FORMAT_LENGTH - 1] = '\0';

  esp_err_t err = set_nvs_str("device", "time_format", device.time_format);
  return err;
}

char *get_device_time_format(void) { return device.time_format; }
