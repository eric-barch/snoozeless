#ifndef BUTTON_H
#define BUTTON_H

class Button {
public:
  Button();

  ~Button();

  bool is_pressed();

private:
  static const char *const TAG;
};

#endif
