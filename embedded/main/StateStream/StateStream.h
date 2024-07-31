#ifndef STATE_STREAM_H
#define STATE_STREAM_H

#include "Alarms.h"
#include "Device.h"
#include "Session.h"

enum StateEvent {
  INITIAL_DEVICE,
  DEVICE_UPDATE,
  INITIAL_ALARMS,
  ALARM_INSERT,
  ALARM_UPDATE,
  ALARM_DELETE,
};

class StateStream {
public:
  StateStream(Session &session, Device &device, Alarms &alarms);

  ~StateStream();

  void on_data(const std::string &response);

private:
  static const char *const TAG;
  static const std::map<const std::string, const StateEvent> events;

  Session &session;
  Device &device;
  Alarms &alarms;

  static void handle_subscribe(void *const pvParameters);

  void subscribe();
  void extract_field(const std::string &response, const std::string &key,
                     std::string &value);
};

#endif // STATE_STREAM_H
