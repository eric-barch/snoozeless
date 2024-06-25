#include "state/real_time.h"
#include "esp_err.h"
#include "nvs.h"
#include "services/real_time.h"

static real_time_t real_time;

esp_err_t initialize_real_time(void) {
  esp_err_t err = get_nvs_int("real_time", "unix", &real_time.unix);
  if (err != ESP_OK) {
    err = calibrate_real_time(NULL);
    if (err != ESP_OK) {
      return err;
    }

    err = set_real_time_unix(real_time.unix);
    if (err != ESP_OK) {
      return err;
    }
  }

  err = get_nvs_int("real_time", "timestamp", &real_time.timestamp);
  if (err != ESP_OK) {
    /**If we have entered this block, timestamp is already stored in the
     * volatile real_time struct as a result of calling `calibrate_real_time`
     * above. We still call `set_real_time_timestamp` to register the timestamp
     * in non-volatile storage. */
    err = set_real_time_timestamp(real_time.timestamp);
    if (err != ESP_OK) {
      return err;
    }
  }

  return err;
}

esp_err_t set_real_time_unix(const int unix) {
  real_time.unix = unix;

  esp_err_t err = set_nvs_int("time", "unix", unix);
  return err;
}

int get_real_time_unix(void) { return real_time.unix; }

esp_err_t set_real_time_timestamp(const int timestamp) {
  real_time.timestamp = timestamp;

  esp_err_t err = set_nvs_int("time", "timestamp", timestamp);
  return err;
}

int get_real_time_timestamp(void) { return real_time.timestamp; }
