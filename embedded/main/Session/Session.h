#ifndef SESSION_H
#define SESSION_H

#include "NonVolatileStorage.h"
#include <esp_err.h>
#include <freertos/idf_additions.h>
#include <stdint.h>
#include <unordered_map>

class Session {
public:
  Session(NonVolatileStorage &non_volatile_storage);
  ~Session();

  std::string get_access_token();
  std::string get_refresh_token();

  void on_data(const std::string &response);

private:
  static const char *const TAG;
  NonVolatileStorage &non_volatile_storage;
  std::string access_token;
  std::string refresh_token;
  SemaphoreHandle_t is_refreshed;

  void set_access_token(const std::string &access_token);
  void set_refresh_token(const std::string &refresh_token);

  esp_err_t refresh();
  static void keep_refreshed(void *const pvParameters);
};

#endif // SESSION_H
