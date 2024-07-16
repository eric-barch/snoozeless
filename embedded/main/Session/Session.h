#ifndef SESSION_H
#define SESSION_H

#include "NvsManager.h"
#include "freertos/idf_additions.h"

class Session {
public:
  Session(NvsManager &nvs_manager);
  ~Session();

  std::string get_access_token();
  std::string get_refresh_token();

private:
  NvsManager &nvs_manager;
  std::string access_token;
  std::string refresh_token;
  SemaphoreHandle_t is_blocked;

  void set_access_token(std::string access_token);
  void set_refresh_token(std::string refresh_token);

  static void refresh_on_data(void *session, const std::string &response);
  void refresh();
  static void keep_refreshed_task(void *pvParameters);
  void keep_refreshed();
};

#endif // SESSION_H
