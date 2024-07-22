#include "Device.h"
#include "ApiRequest.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "NvsManager.h"
#include "Session.h"
#include <cJSON.h>
#include <esp_err.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <freertos/idf_additions.h>

static const char *TAG = "Device";

Device::Device(NvsManager &nvs_manager, Session &session,
               CurrentTime &current_time, Display &display, Buzzer &buzzer)
    : nvs_manager(nvs_manager), session(session), current_time(current_time),
      display(display), buzzer(buzzer) {
  std::string id;
  esp_err_t err = this->nvs_manager.read_string("device", "id", id);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "ID read from NVS: %s", id.c_str());
    this->set_id(id);
  } else {
    ESP_LOGW(TAG, "Error reading ID from NVS: %s.", esp_err_to_name(err));
    this->enroll();
  }

  this->is_subscribed = xSemaphoreCreateBinary();
  xSemaphoreGive(this->is_subscribed);

  this->keep_subscribed();
  this->display.print_current_time();
}

Device::~Device() {}

void Device::set_id(std::string id) {
  this->id = id;
  this->nvs_manager.write_string("device", "id", id);
  ESP_LOGI(TAG, "Set ID: %s", id.c_str());
}

void Device::enroll_on_data(void *device, const std::string &response) {
  Device *self = static_cast<Device *>(device);

  cJSON *json_response = cJSON_Parse(response.c_str());
  if (!json_response) {
    ESP_LOGE(TAG, "Failed to parse JSON response.");
    return;
  }

  cJSON *id_item = cJSON_GetObjectItem(json_response, "id");
  if (!cJSON_IsString(id_item) || (id_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `id` from JSON response");
  } else {
    self->set_id(id_item->valuestring);
  }

  cJSON_Delete(json_response);
}

esp_err_t Device::enroll() {
  ApiRequest post_device_enroll =
      ApiRequest(this->session, this, enroll_on_data, HTTP_METHOD_POST, 60000,
                 "/device/enroll");
  esp_err_t err = post_device_enroll.send();
  return err;
}

void Device::subscribe_on_data(void *device, const std::string &response) {
  std::string short_response = response.substr(0, response.length() - 2);
  ESP_LOGI(TAG, "%s", short_response.c_str());

  Device *self = static_cast<Device *>(device);

  size_t data_pos = response.find("data: ");
  if (data_pos == std::string::npos) {
    ESP_LOGE(TAG, "No valid data field found in Server Sent Event.");
    return;
  }

  /**Length of "data: " is 6. */
  size_t start_pos = data_pos + 6;
  size_t end_pos = response.find('\n', start_pos);
  std::string sse_data = response.substr(start_pos, end_pos - start_pos);

  if (sse_data == "keep-alive") {
    return;
  }

  cJSON *json_data = cJSON_Parse(sse_data.c_str());
  if (!json_data) {
    ESP_LOGE(TAG, "Failed to parse JSON data.");
    return;
  }

  cJSON *time_zone_item = cJSON_GetObjectItem(json_data, "time_zone");
  if (!cJSON_IsString(time_zone_item) ||
      (time_zone_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_zone` from JSON response.");
  } else {
    self->current_time.set_time_zone(time_zone_item->valuestring);
  }

  cJSON *time_format_item = cJSON_GetObjectItem(json_data, "time_format");
  if (!cJSON_IsString(time_format_item) ||
      (time_format_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_format` from JSON response.");
  } else {
    self->current_time.set_format(time_format_item->valuestring);
  }

  cJSON *brightness_item = cJSON_GetObjectItem(json_data, "brightness");
  if (!cJSON_IsNumber(brightness_item)) {
    ESP_LOGE(TAG, "Failed to extract `brightness` from JSON response.");
  } else {
    self->display.set_brightness(brightness_item->valueint);
  }

  cJSON_Delete(json_data);
}

void Device::subscribe() {
  std::string query = "deviceId=" + this->id;
  ApiRequest get_device_state =
      ApiRequest(this->session, this, subscribe_on_data, HTTP_METHOD_GET,
                 300000, "/device/state", query);
  get_device_state.send();
  ESP_LOGI(TAG, "Subscription successful.");
  xSemaphoreGive(this->is_subscribed);
}

void Device::subscribe_task(void *pvParameters) {
  Device *self = static_cast<Device *>(pvParameters);

  ESP_LOGI(TAG, "Subscribing.");

  while (true) {
    self->subscribe();
    ESP_LOGI(TAG, "Subscription failed. Will try to reconnect in one minute.");
    vTaskDelay(pdMS_TO_TICKS(60000));
  }

  vTaskDelete(NULL);
}

void Device::keep_subscribed() {
  xSemaphoreTake(this->is_subscribed, 0);
  xTaskCreate(Device::subscribe_task, "keep_subscribed", 2048, this, 5, NULL);
  xSemaphoreTake(this->is_subscribed, portMAX_DELAY);
}
