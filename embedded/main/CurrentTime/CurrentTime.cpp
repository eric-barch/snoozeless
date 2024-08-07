#include "CurrentTime.h"
#include "ApiRequest.h"
#include "NonVolatileStorage.h"
#include "Session.h"
#include <cJSON.h>
#include <ctime>
#include <esp_err.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <freertos/idf_additions.h>

CurrentTime::CurrentTime(NonVolatileStorage &non_volatile_storage,
                         Session &session)
    : non_volatile_storage(non_volatile_storage), session(session),
      unix_at_calibration(0), ms_at_calibration(0) {
  esp_err_t err =
      non_volatile_storage.read(TAG, "unix_at_cal", unix_at_calibration);
  if (err == ESP_OK) {
    ESP_LOGD(TAG, "Initial Unix at Calibration read from NVS: %d",
             unix_at_calibration);
    set_unix_at_calibration(unix_at_calibration);
  } else {
    ESP_LOGW(TAG, "Error reading initial Unix at Calibration from NVS: %s",
             esp_err_to_name(err));
  }

  err = non_volatile_storage.read(TAG, "ms_at_cal", ms_at_calibration);
  if (err == ESP_OK) {
    ESP_LOGD(TAG, "Initial Milliseconds at Calibration read from NVS: %d",
             ms_at_calibration);
    set_ms_at_calibration(ms_at_calibration);
  } else {
    ESP_LOGW(TAG,
             "Error reading initial Milliseconds at Calibration from NVS: %s",
             esp_err_to_name(err));
  }

  xTaskCreate(CurrentTime::keep_calibrated, "keep_calibrated", 4096, this, 5,
              NULL);
}

CurrentTime::~CurrentTime() { ESP_LOGI(TAG, "Destroy."); }

std::tm CurrentTime::get_time() {
  int seconds_since_calibration =
      (esp_log_timestamp() - ms_at_calibration) / 1000;
  std::time_t unix_time = unix_at_calibration + seconds_since_calibration;
  std::tm time;
  localtime_r(&unix_time, &time);
  return time;
}

void CurrentTime::on_data(const std::string &response) {
  cJSON *const json_response = cJSON_Parse(response.c_str());
  if (!json_response) {
    ESP_LOGE(TAG, "Failed to parse JSON response.");
    cJSON_Delete(json_response);
    return;
  }

  const cJSON *const unix_time_json =
      cJSON_GetObjectItem(json_response, "unix_time");
  if (!cJSON_IsNumber(unix_time_json)) {
    ESP_LOGE(TAG, "Failed to extract `unix_time` from JSON response");
    cJSON_Delete(json_response);
    return;
  }

  set_unix_at_calibration(unix_time_json->valueint);
  set_ms_at_calibration(esp_log_timestamp());

  cJSON_Delete(json_response);
}

const char *const CurrentTime::TAG = "curr_time";

void CurrentTime::set_unix_at_calibration(const int &unix_at_calibration) {
  this->unix_at_calibration = unix_at_calibration;
  non_volatile_storage.write(TAG, "unix_at_cal", unix_at_calibration);
  ESP_LOGI(TAG, "Set Unix at Calibration: %d", unix_at_calibration);
}

void CurrentTime::set_ms_at_calibration(const int &ms_at_calibration) {
  this->ms_at_calibration = ms_at_calibration;
  non_volatile_storage.write(TAG, "ms_at_cal", ms_at_calibration);
  ESP_LOGI(TAG, "Set Milliseconds at Calibration: %d", ms_at_calibration);
}

void CurrentTime::keep_calibrated(void *const pvParameters) {
  CurrentTime *self = static_cast<CurrentTime *>(pvParameters);

  while (true) {
    ESP_LOGI(TAG, "Calibrating.");
    esp_err_t err = self->calibrate();

    while (err != ESP_OK) {
      ESP_LOGI(TAG, "Calibration failed. Will try again in ten seconds.");
      vTaskDelay(pdMS_TO_TICKS(10000));
      err = self->calibrate();
    }

    ESP_LOGI(TAG, "Calibration successful.");

    /**Recalibrate every 24 hours. */
    /**FIXME: Pretty sure this is running way more often than every 24 hours. */
    vTaskDelay(pdMS_TO_TICKS(86400000));
  }

  vTaskDelete(NULL);
}

esp_err_t CurrentTime::calibrate() {
  ApiRequest get_unix_time = ApiRequest<CurrentTime>(
      session, *this, HTTP_METHOD_GET, 60000, "/unix-time", "");
  esp_err_t err = get_unix_time.send_request();
  return err;
}
