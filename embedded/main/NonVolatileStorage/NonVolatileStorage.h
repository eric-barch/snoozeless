#ifndef NON_VOLATILE_STORAGE_H
#define NON_VOLATILE_STORAGE_H

#include <esp_err.h>
#include <nvs.h>
#include <string>

class NonVolatileStorage {
public:
  NonVolatileStorage();
  ~NonVolatileStorage();

  esp_err_t write_key(const std::string &space, const std::string &key,
                      const std::string &in_value);
  esp_err_t read_key(const std::string &space, const std::string &key,
                     std::string &out_value);
  esp_err_t write_key(const std::string &space, const std::string &key,
                      const int &in_value);
  esp_err_t read_key(const std::string &space, const std::string &key,
                     int &out_value);
  esp_err_t erase_key(const std::string &space, const std::string &key);

private:
  static const char *const TAG;
  nvs_handle_t nvs_handle;

  esp_err_t open_namespace(const std::string &space);
};

#endif // NON_VOLATILE_STORAGE_H
