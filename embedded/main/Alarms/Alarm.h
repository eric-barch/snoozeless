#ifndef ALARM_H
#define ALARM_H

#include "NvsManager.h"
#include <cJSON.h>
#include <string>

class Alarm {
public:
  Alarm(NvsManager &nvs_manager, const std::string &id);
  Alarm(NvsManager &nvs_manager, const cJSON *alarm_json);

private:
  NvsManager &nvs_manager;
  std::string id;
  std::string name;
  std::string schedule;
  int time_to_abort;

  void set_id(const std::string &id);
  void set_name(const std::string &name);
  void set_schedule(const std::string &schedule);
  void set_time_to_abort(const int time_to_abort);
};

#endif
