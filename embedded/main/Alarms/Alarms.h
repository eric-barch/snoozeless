#ifndef ALARMS_H
#define ALARMS_H

#include "Alarm.h"
#include "NonVolatileStorage.h"
#include <map>
#include <memory>

class Alarms {
public:
  Alarms(NonVolatileStorage &non_volatile_storage);

  ~Alarms();

  void parse_initial(const std::string &data);
  void parse_insert(const std::string &data);
  void parse_update(const std::string &data);
  void parse_remove(const std::string &data);
  Alarm &get_next();

private:
  static const char *const TAG;

  NonVolatileStorage &non_volatile_storage;
  std::map<const std::string, const std::unique_ptr<Alarm>> alarms;

  void write_ids_to_nvs();
};

#endif // ALARMS_H
