#ifndef SESSION_H
#define SESSION_H

#include "NvsManager.h"

class Session {
public:
  Session(NvsManager &nvs_manager);

  std::string get_auth_bearer_token();
  std::string get_refresh_token();
  void keep_active();

private:
  NvsManager &nvs_manager;
  std::string auth_bearer_token;
  std::string refresh_token;

  void init();
};

#endif // SESSION_H
