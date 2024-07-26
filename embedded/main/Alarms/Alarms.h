#ifndef ALARMS_H
#define ALARMS_H

#include "Alarm.h"
#include "NvsManager.h"
#include <map>
#include <memory>
#include <vector>

class Alarms {
public:
  Alarms(NvsManager &nvs_manager);

  void parse_initial_alarms(const std::string &data);
  void parse_alarm_insert(const std::string &data);
  void parse_alarm_update(const std::string &data);
  void parse_alarm_remove(const std::string &data);

private:
  NvsManager &nvs_manager;
  std::map<std::string, std::unique_ptr<Alarm>> alarms;
};

#endif
