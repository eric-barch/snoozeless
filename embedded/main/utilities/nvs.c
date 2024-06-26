#include "utilities/nvs.h"
#include "esp_err.h"
#include "nvs.h"
#include "nvs_flash.h"

static nvs_handle_t handle;

esp_err_t initialize_nvs(void) {
  esp_err_t err = nvs_flash_init();

  switch (err) {
  case ESP_OK:
    printf("Initialized NVS.\n");
    break;
  case ESP_ERR_NVS_NO_FREE_PAGES:
    /**Fall through. */
  case ESP_ERR_NVS_NEW_VERSION_FOUND:
    err = nvs_flash_erase();
    if (err != ESP_OK) {
      printf("Error erasing NVS: %s\n", esp_err_to_name(err));
      break;
    }

    err = nvs_flash_init();
    if (err != ESP_OK) {
      printf("Error reinitializing NVS: %s\n", esp_err_to_name(err));
      break;
    }

    printf("Erased and reinitialized NVS.\n");
    break;
  default:
    printf("Error initializing NVS: %s\n", esp_err_to_name(err));
    break;
  }

  return err;
}

esp_err_t set_nvs_str(const char *namespace, const char *key,
                      const char *in_value) {
  esp_err_t err = nvs_open(namespace, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_set_str(handle, key, in_value);

  if (err == ESP_OK) {
    err = nvs_commit(handle);
  }

  switch (err) {
  case ESP_OK:
    printf("Set string %s in NVS: %s\n", key, in_value);
    break;
  case ESP_ERR_NVS_NOT_FOUND:
    printf("Key not found in NVS: %s\n", key);
    break;
  default:
    printf("Error setting string in NVS: %s\n", esp_err_to_name(err));
    break;
  }

  return err;
}

esp_err_t get_nvs_str(const char *namespace, const char *key, char *out_value,
                      size_t max_length) {
  esp_err_t err = nvs_open(namespace, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_get_str(handle, key, out_value, &max_length);

  switch (err) {
  case ESP_OK:
    printf("Got string %s from NVS: %s\n", key, out_value);
    break;
  case ESP_ERR_NVS_NOT_FOUND:
    printf("Key not found in NVS: %s\n", key);
    break;
  default:
    printf("Error getting string from NVS: %s\n", esp_err_to_name(err));
    break;
  }

  return err;
}

esp_err_t set_nvs_int(const char *namespace, const char *key, int in_value) {
  esp_err_t err = nvs_open(namespace, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_set_i32(handle, key, (int32_t)in_value);

  if (err == ESP_OK) {
    err = nvs_commit(handle);
  }

  switch (err) {
  case ESP_OK:
    printf("Set integer %s in NVS: %d\n", key, in_value);
    break;
  case ESP_ERR_NVS_NOT_FOUND:
    printf("Key not found in NVS: %s\n", key);
    break;
  default:
    printf("Error setting integer in NVS: %s\n", esp_err_to_name(err));
    break;
  }

  return err;
}

esp_err_t get_nvs_int(const char *namespace, const char *key, int *out_value) {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(namespace, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  int32_t temp_value;
  err = nvs_get_i32(handle, key, &temp_value);

  if (err == ESP_OK) {
    *out_value = (int)temp_value;
    printf("Got integer %s from NVS: %d\n", key, *out_value);
  } else {
    switch (err) {
    case ESP_ERR_NVS_NOT_FOUND:
      printf("Key not found in NVS: %s\n", key);
      break;
    default:
      printf("Error getting integer from NVS: %s\n", esp_err_to_name(err));
      break;
    }
  }

  nvs_close(handle);
  return err;
}

esp_err_t erase_nvs_key(const char *namespace, const char *key) {
  esp_err_t err = nvs_open(namespace, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_erase_key(handle, key);

  switch (err) {
  case ESP_OK:
    printf("Erased key from NVS: %s\n", key);
    break;
  case ESP_ERR_NVS_NOT_FOUND:
    printf("Key not found in NVS: %s\n", key);
    break;
  default:
    printf("Error erasing key from NVS: %s\n", esp_err_to_name(err));
    break;
  }

  return err;
}

void close_nvs(void) {
  nvs_close(handle);
  printf("Closed NVS.\n");
}
