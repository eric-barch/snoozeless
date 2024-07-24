#ifndef ALARMS_H
#define ALARMS_H

#include "Alarm.h"
#include "NvsManager.h"
#include <map>
#include <vector>

class Alarms {
public:
  Alarms(NvsManager &nvs_manager);

  void set_alarms(const std::vector<Alarm *> alarms);

private:
  NvsManager &nvs_manager;
  std::map<std::string, Alarm *> alarms;
};

#endif
