#ifndef SESSION_H
#define SESSION_H

#include "NvsManager.h"

class Session {
private:
  NvsManager &nvs_manager;
  std::string auth_bearer_token;
  std::string refresh_token;

public:
  Session(NvsManager &nvs_manager);
  ~Session();

  std::string get_auth_bearer_token();
  std::string get_refresh_token();
  void keep_active();
};

#endif // SESSION_H
