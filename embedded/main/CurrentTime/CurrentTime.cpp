#include "CurrentTime/CurrentTime.h"
#include "NvsManager.h"
#include "Session.h"
#include "esp_log.h"

static const char *TAG = "CurrentTime";

CurrentTime::CurrentTime(NvsManager &nvs_manager, Session &session)
    : nvs_manager(nvs_manager), session(session) {
  this->init();
}

void CurrentTime::init() {
  /**We'll always calibrate the time on `init`, but we may as well read any
   * saved calibration here. If something goes wrong with the calibration API
   * request, we'll at least have the time at last shutdown. */
  esp_err_t err = this->nvs_manager.read_int("current_time", "cal_unix",
                                             this->unix_at_calibration);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Initial Unix at Calibration read from NVS: %d",
             this->unix_at_calibration);
  } else {
    ESP_LOGW(TAG, "Error reading initial Unix at Calibration from NVS: %s",
             esp_err_to_name(err));
  }

  err = this->nvs_manager.read_int("current_time", "cal_ms",
                                   this->ms_at_calibration);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Initial Milliseconds at Calibration read from NVS: %d",
             this->ms_at_calibration);
  } else {
    ESP_LOGW(TAG,
             "Error reading initial Milliseconds at Calibration from NVS: %s",
             esp_err_to_name(err));
  }

  err = this->nvs_manager.read_int("current_time", "utc_offset",
                                   this->utc_offset);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "UTC Offset read from NVS: %d", this->utc_offset);
  } else {
    ESP_LOGW(TAG, "Error reading UTC Offset from NVS: %s",
             esp_err_to_name(err));
  }

  err = this->nvs_manager.read_string("current_time", "format", this->format);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Format read from NVS: %s", this->format.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading format from NVS: %s", esp_err_to_name(err));
  }
}
