#ifndef ALARMS_H
#define ALARMS_H

#include "Alarm.h"
#include "NonVolatileStorage.h"
#include <map>
#include <memory>
#include <vector>

class Alarms {
public:
  Alarms(NonVolatileStorage &non_volatile_storage);

  void parse_initial_alarms(const std::string &data);
  void parse_alarm_insert(const std::string &data);
  void parse_alarm_update(const std::string &data);
  void parse_alarm_remove(const std::string &data);

private:
  NonVolatileStorage &non_volatile_storage;
  std::map<std::string, std::unique_ptr<Alarm>> alarms;
};

#endif
