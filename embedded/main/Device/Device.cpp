#include "Device.h"
#include "ApiRequest.h"
#include "NvsManager.h"
#include "Session.h"
#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "Device";

Device::Device(NvsManager &nvs_manager, Session &session)
    : nvs_manager(nvs_manager), session(session) {
  this->init();
}

void Device::enroll_on_data(void *device_context, const std::string &response) {
  Device *self = static_cast<Device *>(device_context);
  ESP_LOGI(TAG, "Response: %s", response.c_str());

  cJSON *json_response = cJSON_Parse(response.c_str());
  if (!json_response) {
    ESP_LOGE(TAG, "Failed to parse JSON response.");
    return;
  }

  cJSON *id_item = cJSON_GetObjectItem(json_response, "id");
  if (!cJSON_IsString(id_item) || (id_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `id` from JSON response");
    return;
  }

  self->id = std::string(id_item->valuestring);
  self->nvs_manager.write_string("device", "id", self->id);
  ESP_LOGI(TAG, "Device ID set to: %s", self->id.c_str());

  cJSON_Delete(json_response);
}

void Device::enroll() {
  ApiRequest post_device_register =
      ApiRequest(session, HTTP_METHOD_POST, 60000, "/device/register", "", this,
                 enroll_on_data);
  post_device_register.send_request();
  /**NOTE: `enroll` does not return until `post_device_register` allows itself
   * to destruct. */
}

void Device::init() {
  esp_err_t err = this->nvs_manager.read_string("device", "id", this->id);

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Device ID read from NVS: %s", this->id.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading Device ID from NVS: %s. Enrolling device.",
             esp_err_to_name(err));
    this->enroll();
  }
}
