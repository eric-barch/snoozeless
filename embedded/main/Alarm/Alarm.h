#ifndef ALARM_H
#define ALARM_H

#include "NvsManager.h"
#include <string>

class Alarm {
public:
  Alarm(NvsManager &nvs_manager, std::string id);
  ~Alarm();

private:
  NvsManager &nvs_manager;
  std::string id;
  std::string name;
  int time_of_day;
  int time_to_abort;
  std::string recurrence;

  void set_name(std::string name);
  void set_time_of_day(int time_of_day);
  void set_time_to_abort(int time_to_abort);
  void set_recurrence(std::string recurrence);
};

#endif
