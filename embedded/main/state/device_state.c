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

  err = get_nvs_int("device", "utc_offset", &device.utc_offset);
  if (err != ESP_OK) {
    /**If this fails, we have already called `register_device` above, so the
     * utc_offset is stored in the volatile device struct. We still call
     * `set_device_utc_offset` to register the utc_offset in non-volatile
     * storage for persistence across reboots. */
    err = set_device_utc_offset(device.utc_offset);
    if (err != ESP_OK) {
      return err;
    }
  }

  err = get_nvs_str("device", "time_format", device.time_format,
                    MAX_TIME_FORMAT_LENGTH);
  if (err != ESP_OK) {
    /**Same as above. */
    err = set_device_time_format(device.time_format);
    if (err != ESP_OK) {
      return err;
    }
  }

  err = get_nvs_int("device", "brightness", &device.brightness);
  if (err != ESP_OK) {
    /**Same as above. */
    err = set_device_utc_offset(device.brightness);
    if (err != ESP_OK) {
      return err;
    }
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

esp_err_t set_device_utc_offset(int utc_offset) {
  device.utc_offset = utc_offset;

  esp_err_t err = set_nvs_int("device", "utc_offset", utc_offset);
  return err;
}

int get_device_utc_offset(void) { return device.utc_offset; }

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

esp_err_t set_device_brightness(int brightness) {
  device.brightness = brightness;

  esp_err_t err = set_nvs_int("device", "brightness", brightness);
  return err;
}

int get_device_brightness(void) { return device.brightness; }
