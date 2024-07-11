#include "CurrentTime/CurrentTime.h"
#include "ApiRequest.h"
#include "NvsManager.h"
#include "Session.h"
#include "cJSON.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include <ctime>

static const char *TAG = "CurrentTime";

CurrentTime::CurrentTime(NvsManager &nvs_manager, Session &session)
    : nvs_manager(nvs_manager), session(session) {
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

  err = this->nvs_manager.read_string("current_time", "time_zone",
                                      this->time_zone);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Time zone read from NVS: %s", this->time_zone.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading time zone from NVS: %s", esp_err_to_name(err));
  }

  err = this->nvs_manager.read_string("current_time", "format", this->format);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Format read from NVS: %s", this->format.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading format from NVS: %s", esp_err_to_name(err));
  }

  this->calibrate();
}

CurrentTime::~CurrentTime() { ESP_LOGI(TAG, "Destruct."); }

void CurrentTime::set_unix_at_calibration(int unix_at_calibration) {
  this->unix_at_calibration = unix_at_calibration;
  this->nvs_manager.write_int("current_time", "unix_at_cal",
                              unix_at_calibration);
  ESP_LOGI(TAG, "Set Unix at Calibration: %d", unix_at_calibration);
}

void CurrentTime::set_ms_at_calibration(int ms_at_calibration) {
  this->ms_at_calibration = ms_at_calibration;
  this->nvs_manager.write_int("current_time", "ms_at_cal", ms_at_calibration);
  ESP_LOGI(TAG, "Set Milliseconds at Calibration: %d", ms_at_calibration);
}

void CurrentTime::set_time_zone(const std::string &time_zone) {
  setenv("TZ", time_zone.c_str(), 1);
  tzset();
  this->nvs_manager.write_string("current_time", "time_zone", time_zone);
  ESP_LOGI(TAG, "Set time zone: %s", time_zone.c_str());
}

void CurrentTime::set_format(const std::string &format) {
  this->format = format;
  this->nvs_manager.write_string("current_time", "format", format);
  ESP_LOGI(TAG, "Set format: %s", format.c_str());
}

std::tm CurrentTime::get_time() {
  int seconds_since_calibration =
      (esp_log_timestamp() - this->ms_at_calibration) / 1000;
  std::time_t unix_time = this->unix_at_calibration + seconds_since_calibration;
  std::tm time;
  localtime_r(&unix_time, &time);
  return time;
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

  self->set_unix_at_calibration(unix_time_item->valueint);
  self->set_ms_at_calibration(esp_log_timestamp());
}

void CurrentTime::calibrate() {
  ApiRequest get_unix_time =
      ApiRequest(session, HTTP_METHOD_GET, 60000, "/unix-time", "", this,
                 calibrate_on_data);
  get_unix_time.send_request();
}
