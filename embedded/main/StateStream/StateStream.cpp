#include "StateStream.h"
#include "Alarms.h"
#include "ApiRequest.h"
#include "Device.h"
#include "Session.h"
#include <esp_log.h>

StateStream::StateStream(Session &session, Device &device, Alarms &alarms)
    : session(session), device(device), alarms(alarms) {
  xTaskCreate(StateStream::keep_subscribed, "keep_subscribed", 4096, this, 5,
              NULL);
};

StateStream::~StateStream() { ESP_LOGI(TAG, "Destroy."); }

void StateStream::on_data(const std::string &response) {
  /**Trim trailing newlines. */
  const std::string trimmed_response =
      response.substr(0, response.length() - 2);

  std::string event;
  extract_field(trimmed_response, "event: ", event);
  ESP_LOGI(TAG, "event: %s", event.c_str());

  if (event == "keep-alive") {
    return;
  }

  std::string data;
  extract_field(trimmed_response, "data: ", data);
  ESP_LOGI(TAG, "data: %s", data.c_str());

  StateStreamEvent state_stream_event = events.at(event);

  switch (state_stream_event) {
  case INITIAL_DEVICE:
    device.parse(data);
    break;
  case DEVICE_UPDATE:
    device.parse(data);
    break;
  case INITIAL_ALARMS:
    alarms.parse_initial(data);
    break;
  case ALARM_INSERT:
    alarms.parse_insert(data);
    break;
  case ALARM_UPDATE:
    alarms.parse_update(data);
    break;
  case ALARM_DELETE:
    alarms.parse_remove(data);
    break;
  default:
    ESP_LOGE(TAG, "Unknown state stream event: %s", data.c_str());
    break;
  }
}

const char *const StateStream::TAG = "state_stream";

const std::map<const std::string, const StateStreamEvent> StateStream::events =
    {
        {"initial-device", INITIAL_DEVICE}, {"device-update", DEVICE_UPDATE},
        {"initial-alarms", INITIAL_ALARMS}, {"alarm-insert", ALARM_INSERT},
        {"alarm-update", ALARM_UPDATE},     {"alarm-delete", ALARM_DELETE},
};

void StateStream::keep_subscribed(void *const pvParameters) {
  StateStream *self = static_cast<StateStream *>(pvParameters);

  while (true) {
    self->subscribe();
    ESP_LOGI(TAG, "Subscription failed. Will try to reconnect in ten seconds.");
    vTaskDelay(pdMS_TO_TICKS(10000));
  }

  vTaskDelete(NULL);
}

void StateStream::subscribe() {
  ESP_LOGI(TAG, "Subscribing.");
  const std::string query = "deviceId=" + device.get_id();
  ApiRequest get_device_state = ApiRequest<StateStream>(
      session, *this, HTTP_METHOD_GET, 300000, "/device/state", query);
  get_device_state.send_request();
  ESP_LOGI(TAG, "Subscription successful.");
}

void StateStream::extract_field(const std::string &response,
                                const std::string &key, std::string &value) {
  const size_t pos = response.find(key);
  if (pos == std::string::npos) {
    ESP_LOGE(TAG, "Error finding response field %s", key.c_str());
    return;
  }

  const size_t start_pos = pos + key.length();
  const size_t end_pos = response.find('\n', start_pos);

  value = response.substr(start_pos, end_pos - start_pos);
}
