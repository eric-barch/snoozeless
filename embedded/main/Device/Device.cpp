#include "Device.h"
#include "ApiRequest.h"
#include "NvsManager.h"
#include "Session.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "Device";

Device::Device(NvsManager &nvs_manager, Session &session)
    : nvs_manager(nvs_manager), session(session) {
  this->init();
}

void Device::enroll_on_data(void *device_context, const std::string &response) {
  Device *self = static_cast<Device *>(device_context);
  /**TODO: Handle response. */
  ESP_LOGI(TAG, "response: %s", response.c_str());
}

void Device::enroll() {
  ApiRequest post_device_register =
      ApiRequest(session, HTTP_METHOD_POST, 60000, "/device/register", "", this,
                 enroll_on_data);
  post_device_register.send_request();
  /**NOTE: `enroll` will not return until `post_device_register` allows itself
   * to destruct after the HTTP connection terminates. This is fine because we
   * don't want `main` to resume until `device` is fully initialized. For a
   * persistent and/or asynchronous request, the `ApiRequest` (in this case,
   * `post_device_register`) should live in a higher scope (e.g. as an instance
   * property on `Device`) so the calling function can return while the request
   * continues to run in the background. */
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
