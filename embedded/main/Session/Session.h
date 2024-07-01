#ifndef SESSION_H
#define SESSION_H

#include "NvsManager.h"

class Session {
private:
  std::string auth_bearer_token;
  std::string refresh_token;
  NvsManager &nvs_manager;

public:
  Session(NvsManager &nvs_manager);
  ~Session();

  void keep_active();
};

#endif // SESSION_H
