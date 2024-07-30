#ifndef SESSION_H
#define SESSION_H

#include "NonVolatileStorage.h"
#include <esp_err.h>

class Session {
public:
  Session(NonVolatileStorage &non_volatile_storage);

  ~Session();

  std::string get_access_token() const;
  std::string get_refresh_token() const;

  void on_data(const std::string &response);

private:
  static const char *const TAG;

  NonVolatileStorage &non_volatile_storage;
  std::string access_token;
  std::string refresh_token;

  void set_access_token(const std::string &access_token);
  void set_refresh_token(const std::string &refresh_token);

  static void handle_refresh(void *const pvParameters);

  esp_err_t refresh();
};

#endif // SESSION_H
