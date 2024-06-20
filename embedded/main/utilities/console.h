#ifndef CONSOLE_UTILITY
#define CONSOLE_UTILITY

#include "esp_err.h"

esp_err_t initialize_console(void);

esp_err_t get_console_str(const char *prompt, char *out_value,
                          size_t max_length);

void close_console(void);

#endif
