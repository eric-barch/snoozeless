#ifndef DISPLAY_H
#define DISPLAY_H

#include "CurrentTime.h"

class Display {
public:
  Display(CurrentTime &current_time);

private:
  CurrentTime &current_time;
  bool top_indicator;
  bool bottom_indicator;
  bool colon;
  bool apostrophe;
  int brightness;
  /** Each interval (e.g. hours "place", minutes "place") is two characters
   * plus a null terminator. */
  char major_interval[3];
  char minor_interval[3];

  void set_major_interval(const std::string &major_interval);
  void set_minor_interval(const std::string &minor_interval);

  void init();
};

#endif
