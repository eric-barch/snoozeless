#include "CurrentTime/CurrentTime.h"
#include "ApiRequest.h"
#include "NvsManager.h"
#include "Session.h"
#include "cJSON.h"
#include "esp_http_client.h"
#include "esp_log.h"

static const char *TAG = "CurrentTime";

CurrentTime::CurrentTime(NvsManager &nvs_manager, Session &session)
    : nvs_manager(nvs_manager), session(session) {
  this->init();
}

void CurrentTime::calibrate_on_data(void *current_time_instance,
                                    const std::string &response) {
  CurrentTime *self = static_cast<CurrentTime *>(current_time_instance);
  ESP_LOGI(TAG, "Response: %s", response.c_str());

  cJSON *json_response = cJSON_Parse(response.c_str());
  if (!json_response) {
    ESP_LOGE(TAG, "Failed to parse JSON response.");
    return;
  }

  cJSON *unix_time_item = cJSON_GetObjectItem(json_response, "unix_time");
  if (!cJSON_IsNumber(unix_time_item)) {
    ESP_LOGE(TAG, "Failed to extract `unix_time` from JSON response");
    return;
  }

  self->unix_at_calibration = unix_time_item->valueint;
  self->nvs_manager.write_int("current_time", "cal_unix",
                              self->unix_at_calibration);
  ESP_LOGI(TAG, "Unix at Calibration set to: %d", self->unix_at_calibration);

  self->ms_at_calibration = esp_log_timestamp();
  self->nvs_manager.write_int("current_time", "cal_ms",
                              self->ms_at_calibration);
  ESP_LOGI(TAG, "Milliseconds at Calibration set to: %u",
           self->ms_at_calibration);
}

void CurrentTime::calibrate() {
  ApiRequest get_unix_time =
      ApiRequest(session, HTTP_METHOD_GET, 60000, "/unix-time", "", this,
                 calibrate_on_data);
  get_unix_time.send_request();
}

void CurrentTime::init() {
  /**We'll always calibrate the time on `init`, but we may as well read any
   * saved calibration here. If something goes wrong with the calibration API
   * request, we'll at least have the time at last shutdown. */
  esp_err_t err = this->nvs_manager.read_int("current_time", "cal_unix",
                                             this->unix_at_calibration);
  if (err == ESP_OK) {
    ESP_LOGI(TAG,
             "Initial Unix at Calibration read from NVS: %d. Will still try to "
             "calibrate.",
             this->unix_at_calibration);
  } else {
    ESP_LOGW(TAG, "Error reading initial Unix at Calibration from NVS: %s",
             esp_err_to_name(err));
  }

  err = this->nvs_manager.read_int("current_time", "cal_ms",
                                   this->ms_at_calibration);
  if (err == ESP_OK) {
    ESP_LOGI(TAG,
             "Initial Milliseconds at Calibration read from NVS: %d. Will "
             "still try to calibrate.",
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

  this->calibrate();
}
