#ifndef SESSION_H
#define SESSION_H

#include "NvsManager.h"

class Session {
public:
  Session(NvsManager &nvs_manager);

  std::string get_auth_bearer_token();
  std::string get_refresh_token();

private:
  NvsManager &nvs_manager;
  std::string auth_bearer_token;
  std::string refresh_token;

  void set_auth_bearer_token(std::string auth_bearer_token);
  void set_refresh_token(std::string refresh_token);

  static void refresh_on_data(void *session_instance,
                              const std::string &response);
  void refresh();
  void init();
};

#endif // SESSION_H
