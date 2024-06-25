#ifndef CONSOLE_UTILITY_H
#define CONSOLE_UTILITY_H

#define MAX_PROMPT_LENGTH 300
#define MAX_INPUT_LENGTH 1000

#include "esp_err.h"

esp_err_t initialize_console(void);

esp_err_t get_console_str(const char *prompt, char *out_value,
                          size_t max_length);

void close_console(void);

#endif
