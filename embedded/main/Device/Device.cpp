#include "Device.h"
#include "ApiRequest.h"
#include "CurrentTime.h"
#include "NvsManager.h"
#include "Session.h"
#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "Device";

Device::Device(NvsManager &nvs_manager, Session &session,
               CurrentTime &current_time)
    : nvs_manager(nvs_manager), session(session), current_time(current_time) {
  this->init();
}

void Device::set_id(std::string id) {
  this->id = id;
  this->nvs_manager.write_string("device", "id", id);
  ESP_LOGI(TAG, "Set ID: %s", id.c_str());
}

void Device::enroll_on_data(void *device_instance,
                            const std::string &response) {
  Device *self = static_cast<Device *>(device_instance);
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

  self->set_id(id_item->valuestring);

  cJSON_Delete(json_response);
}

/**NOTE: Does not return until `post_device_register` destructs. */
void Device::enroll() {
  ApiRequest post_device_register =
      ApiRequest(session, HTTP_METHOD_POST, 60000, "/device/register", "", this,
                 enroll_on_data);
  post_device_register.send_request();
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
