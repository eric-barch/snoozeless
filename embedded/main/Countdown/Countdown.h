#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <ctime>

class Countdown {
  Countdown(const int &duration);

  ~Countdown();

  int get_seconds_remaining();

private:
  static const char *const TAG;

  const int duration;
  const int ms_at_start;
};

#endif
