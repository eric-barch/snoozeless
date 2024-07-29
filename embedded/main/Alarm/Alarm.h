#ifndef ALARM_H
#define ALARM_H

#include "NonVolatileStorage.h"
#include <cJSON.h>
#include <string>

class Alarm {
public:
  Alarm(NonVolatileStorage &non_volatile_storage, const std::string &id);
  Alarm(NonVolatileStorage &non_volatile_storage,
        const cJSON *const alarm_json);
  ~Alarm();

private:
  static const char *const TAG;
  NonVolatileStorage &non_volatile_storage;
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
