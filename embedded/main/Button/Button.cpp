#include "Button.h"
#include <esp_log.h>

Button::Button() {}

Button::~Button() { ESP_LOGI(TAG, "Destroy."); }

bool Button::is_pressed() { return false; }

const char *const Button::TAG = "button";
