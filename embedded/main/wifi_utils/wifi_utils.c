#include "wifi_utils.h"
#include "console_utils.h"
#include "esp_err.h"
#include "esp_event_base.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/idf_additions.h"
#include "nvs_utils.h"
#include <string.h>

#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define WIFI_MAX_SSID_LENGTH 32
#define WIFI_MAX_PASSWORD_LENGTH 64
#define WIFI_MAX_RETRY 5
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static char *TAG = "wifi_utils";

typedef struct {
  char ssid[WIFI_MAX_SSID_LENGTH];
  char password[WIFI_MAX_PASSWORD_LENGTH];
} wifi_credentials_t;

static EventGroupHandle_t s_wifi_event_group;
static wifi_credentials_t wifi_credentials;

esp_err_t get_wifi_credentials(void) {
  esp_err_t err = open_nvs_namespace("wifi_cred");
  if (err != ESP_OK) {
    return err;
  }

  err = get_nvs_str("ssid", wifi_credentials.ssid, WIFI_MAX_SSID_LENGTH);
  if (err != ESP_OK) {
    err = get_console_str("Enter WiFi SSID: ", wifi_credentials.ssid,
                          WIFI_MAX_SSID_LENGTH);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str("ssid", wifi_credentials.ssid);
    if (err != ESP_OK) {
      return err;
    }
  }

  err = get_nvs_str("password", wifi_credentials.password,
                    WIFI_MAX_PASSWORD_LENGTH);
  if (err != ESP_OK) {
    err = get_console_str("Enter WiFi password: ", wifi_credentials.password,
                          WIFI_MAX_PASSWORD_LENGTH);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str("password", wifi_credentials.password);
    if (err != ESP_OK) {
      return err;
    }
  }

  return err;
}

void wifi_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data) {
  static int s_retry_count = 0;

  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (s_retry_count < WIFI_MAX_RETRY) {
      esp_wifi_connect();
      s_retry_count++;
      ESP_LOGI(TAG, "Trying to connect to access point again.");
    } else {
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
    ESP_LOGI(TAG, "Failed to connect to access point.");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
    s_retry_count = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

esp_err_t connect_to_wifi(void) {
  s_wifi_event_group = xEventGroupCreate();

  esp_err_t err = esp_netif_init();
  if (err != ESP_OK) {
    return err;
  }

  err = esp_event_loop_create_default();
  if (err != ESP_OK) {
    return err;
  }

  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

  err = esp_wifi_init(&cfg);
  if (err != ESP_OK) {
    return err;
  }

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;

  err = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                            &wifi_event_handler, NULL,
                                            &instance_any_id);
  if (err != ESP_OK) {
    return err;
  }

  err = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                            &wifi_event_handler, NULL,
                                            &instance_got_ip);
  if (err != ESP_OK) {
    return err;
  }

  wifi_config_t wifi_config = {
      .sta =
          {
              .ssid = "",
              .password = "",
              .threshold.authmode = WIFI_SCAN_AUTH_MODE_THRESHOLD,
              .sae_pwe_h2e = WIFI_SAE_MODE,
          },
  };

  strncpy((char *)wifi_config.sta.ssid, wifi_credentials.ssid,
          sizeof(wifi_config.sta.ssid) - 1);
  strncpy((char *)wifi_config.sta.password, wifi_credentials.password,
          sizeof(wifi_config.sta.password) - 1);

  err = esp_wifi_set_mode(WIFI_MODE_STA);
  if (err != ESP_OK) {
    return err;
  }

  err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
  if (err != ESP_OK) {
    return err;
  }

  err = esp_wifi_start();
  if (err != ESP_OK) {
    return err;
  }

  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE, pdFALSE, portMAX_DELAY);

  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "Connected to SSID %s with password %s",
             wifi_credentials.ssid, wifi_credentials.password);
    return ESP_OK;
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGI(TAG, "Failed to connect to SSID %s with password %s",
             wifi_credentials.ssid, wifi_credentials.password);
    return ESP_FAIL;
  } else {
    ESP_LOGE(TAG, "Unexpected event connecting to SSID %s with password %s",
             wifi_credentials.ssid, wifi_credentials.password);
    return ESP_FAIL;
  }
}

esp_err_t initialize_wifi(void) {
  esp_err_t err = get_wifi_credentials();
  if (err != ESP_OK) {
    return err;
  }

  err = connect_to_wifi();
  return err;
}
