#ifndef ALARM_H
#define ALARM_H

#include "NvsManager.h"
#include <string>

class Alarm {
public:
  Alarm(NvsManager &nvs_manager, std::string id);

private:
  NvsManager &nvs_manager;
  std::string id;
  std::string name;
  std::string schedule;
  int time_to_abort;

  void set_name(std::string name);
  void set_schedule(std::string schedule);
  void set_time_to_abort(int time_to_abort);
};

#endif
