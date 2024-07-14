#ifndef CURRENT_TIME_H
#define CURRENT_TIME_H

#include "NvsManager.h"
#include "Session.h"
#include <ctime>
#include <string>

class CurrentTime {
public:
  CurrentTime(NvsManager &nvs_manager, Session &session);
  ~CurrentTime();

  void set_time_zone(const std::string &time_zone);
  void set_format(const std::string &format);
  std::tm get_time();

private:
  NvsManager &nvs_manager;
  Session &session;
  int unix_at_calibration;
  int ms_at_calibration;
  std::string time_zone;
  std::string format;

  void set_unix_at_calibration(int unix_at_calibration);
  void set_ms_at_calibration(int ms_at_calibration);

  static void calibrate_on_data(void *current_time,
                                const std::string &response);
  void calibrate();
};

#endif
