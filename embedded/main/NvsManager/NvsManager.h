#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include "esp_err.h"
#include "nvs.h"
#include <string>

class NvsManager {
public:
  NvsManager();
  ~NvsManager();

  esp_err_t write_string(const std::string &nvs_namespace,
                         const std::string &key, const std::string &in_value);
  esp_err_t read_string(const std::string &nvs_namespace,
                        const std::string &key, std::string &out_value);
  esp_err_t write_int(const std::string &nvs_namespace, const std::string &key,
                      const int &in_value);
  esp_err_t read_int(const std::string &nvs_namespace, const std::string &key,
                     int &out_value);
  esp_err_t erase_key(const std::string &nvs_namespace, const std::string &key);

private:
  nvs_handle_t nvs_handle;
};

#endif // NVS_MANAGER_H
