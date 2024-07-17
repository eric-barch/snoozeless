#ifndef CURRENT_TIME_H
#define CURRENT_TIME_H

#include "NvsManager.h"
#include "Session.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"
#include <ctime>
#include <string>

class CurrentTime {
public:
  CurrentTime(NvsManager &nvs_manager, Session &session);
  ~CurrentTime();

  void set_time_zone(const std::string &time_zone);
  void set_format(const std::string &format);
  std::string get_format();
  std::tm get_time();

private:
  NvsManager &nvs_manager;
  Session &session;
  int unix_at_calibration;
  int ms_at_calibration;
  std::string time_zone;
  std::string format;
  SemaphoreHandle_t is_calibrated;

  void set_unix_at_calibration(int unix_at_calibration);
  void set_ms_at_calibration(int ms_at_calibration);

  static void calibrate_on_data(void *current_time,
                                const std::string &response);
  esp_err_t calibrate();
  static void keep_calibrated_task(void *pvParameters);
  void keep_calibrated();
};

#endif
