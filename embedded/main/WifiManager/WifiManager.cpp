#include "WifiManager.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <string.h>

static const char *TAG = "WifiManager";

WifiManager::WifiManager(NvsManager &nvs_manager)
    : nvs_manager(nvs_manager), wifi_event_group(nullptr), retry_count(0) {
  esp_err_t err = nvs_manager.read_string("wifi_cred", "ssid", ssid);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "SSID not found in NVS. Using config.");
    ssid = CONFIG_WIFI_SSID;
    nvs_manager.write_string("wifi_cred", "ssid", ssid);
  }

  err = nvs_manager.read_string("wifi_cred", "password", password);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "Password not found in NVS. Using config.");
    password = CONFIG_WIFI_PASSWORD;
    nvs_manager.write_string("wifi_cred", "password", password);
  }
}

WifiManager::~WifiManager() {
  if (wifi_event_group) {
    vEventGroupDelete(wifi_event_group);
  }
}

void WifiManager::wifi_event_handler(void *arg, esp_event_base_t event_base,
                                     int32_t event_id, void *event_data) {
  WifiManager *wifi_manager = static_cast<WifiManager *>(arg);

  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (wifi_manager->retry_count < MAX_RETRY) {
      esp_wifi_connect();
      wifi_manager->retry_count++;
      ESP_LOGI(TAG, "Trying to connect to access point again.");
    } else {
      xEventGroupSetBits(wifi_manager->wifi_event_group, WIFI_FAIL_BIT);
    }
    ESP_LOGW(TAG, "Failed to connect to access point.");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    wifi_manager->retry_count = 0;
    xEventGroupSetBits(wifi_manager->wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

void WifiManager::initialize() {
  wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiManager::wifi_event_handler, this,
      NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &WifiManager::wifi_event_handler, this,
      NULL));

  wifi_config_t wifi_config = {};
  strncpy(reinterpret_cast<char *>(wifi_config.sta.ssid), ssid.c_str(),
          sizeof(wifi_config.sta.ssid) - 1);
  strncpy(reinterpret_cast<char *>(wifi_config.sta.password), password.c_str(),
          sizeof(wifi_config.sta.password) - 1);
  wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
  wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
}

esp_err_t WifiManager::connect() {
  initialize();

  EventBits_t bits =
      xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                          pdFALSE, pdFALSE, portMAX_DELAY);

  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "Connected to SSID %s", ssid.c_str());
    return ESP_OK;
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGE(TAG, "Failed to connect to SSID %s", ssid.c_str());
    return ESP_FAIL;
  } else {
    ESP_LOGE(TAG, "Unexpected event connecting to SSID %s", ssid.c_str());
    return ESP_FAIL;
  }
}

esp_err_t WifiManager::disconnect() {
  ESP_ERROR_CHECK(esp_wifi_stop());
  ESP_LOGI(TAG, "Disconnected from Wi-Fi");
  return ESP_OK;
}
