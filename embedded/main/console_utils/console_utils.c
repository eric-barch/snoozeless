#include "console_utils.h"
#include "driver/uart.h"
#include "esp_console.h"
#include "esp_err.h"
#include "esp_vfs_dev.h"
#include "linenoise/linenoise.h"
#include <stdio.h>

#define MAX_PROMPT_LENGTH 300
#define MAX_INPUT_LENGTH 1000

esp_err_t initialize_console(void) {
  fflush(stdout);
  fsync(fileno(stdout));

  setvbuf(stdin, NULL, _IONBF, 0);

  esp_err_t err = esp_vfs_dev_uart_port_set_rx_line_endings(
      CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
  if (err != ESP_OK) {
    return err;
  }

  err = esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM,
                                                  ESP_LINE_ENDINGS_CRLF);
  if (err != ESP_OK) {
    return err;
  }

  const uart_config_t uart_config = {
      .baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .source_clk = UART_SCLK_REF_TICK,
  };

  err = uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0);
  if (err != ESP_OK) {
    return err;
  }

  err = uart_param_config(CONFIG_ESP_CONSOLE_UART_NUM, &uart_config);
  if (err != ESP_OK) {
    return err;
  }

  esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

  esp_console_config_t console_config = {
      .max_cmdline_length = MAX_INPUT_LENGTH,
  };

  err = esp_console_init(&console_config);
  if (err != ESP_OK) {
    return err;
  }

  linenoiseSetMaxLineLen(console_config.max_cmdline_length);
  linenoiseAllowEmpty(false);

  return ESP_OK;
}

esp_err_t get_console_str(const char *prompt, char *out_value,
                          size_t max_length) {
  if (strlen(prompt) >= MAX_PROMPT_LENGTH) {
    printf("Prompt exceeds max length.\n");
    return ESP_ERR_INVALID_ARG;
  }

  char *input = linenoise(prompt);

  if (input == NULL) {
    printf("Input is null.\n");
    return ESP_FAIL;
  }

  if (strlen(input) >= max_length || strlen(input) >= MAX_INPUT_LENGTH) {
    printf("Input exceeds max length.\n");
    linenoiseFree(input);
    return ESP_ERR_INVALID_SIZE;
  }

  strncpy(out_value, input, max_length);
  out_value[max_length - 1] = (char){0};

  linenoiseFree(input);

  return ESP_OK;
}

void close_console(void) {
  linenoiseHistoryFree();
  esp_console_deinit();
}
