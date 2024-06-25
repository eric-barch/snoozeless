#include "state/real_time.h"
#include "nvs.h"

static real_time_t real_time;

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
