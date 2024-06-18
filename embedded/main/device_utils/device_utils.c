#include "device_utils.h"
#include "console_utils.h"
#include "esp_err.h"
#include "nvs_utils.h"

#define MAX_ID_LENGTH 50
#define MAX_TIME_FORMAT_LENGTH 10

typedef struct {
  char id[MAX_ID_LENGTH];
  char time_format[MAX_TIME_FORMAT_LENGTH];
  int utc_offset;
  int brightness;
} device_t;

static device_t device;

esp_err_t initialize_device_state(void) {
  esp_err_t err = open_nvs_namespace("device");
  if (err != ESP_OK) {
    return err;
  }

  err = get_nvs_str("id", device.id, MAX_ID_LENGTH);
  if (err != ESP_OK) {
    err = get_console_str("Enter device ID: ", device.id, MAX_ID_LENGTH);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str("id", device.id);
    if (err != ESP_OK) {
      return err;
    }
  }

  return err;
};

const char *get_device_id(void) { return device.id; }
