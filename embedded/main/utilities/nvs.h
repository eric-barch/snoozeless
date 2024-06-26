#ifndef NVS_UTILITY_H
#define NVS_UTILITY_H

#include "esp_err.h"

esp_err_t initialize_nvs(void);

esp_err_t set_nvs_str(const char *namespace, const char *key,
                      const char *in_value);

esp_err_t get_nvs_str(const char *namespace, const char *key, char *out_value,
                      size_t max_length);

esp_err_t set_nvs_int(const char *namespace, const char *key, int in_value);

esp_err_t get_nvs_int(const char *namespace, const char *key, int *out_value);

esp_err_t erase_nvs_key(const char *namespace, const char *key);

void close_nvs(void);

#endif
