#include "console_utils.h"
#include "driver/uart.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "linenoise/linenoise.h"
#include <stdio.h>

void initialize_console(void) {
  /* Flush stdout before reconfiguring. */
  fflush(stdout);
  fsync(fileno(stdout));

  /* Disable buffering on stdin. */
  setvbuf(stdin, NULL, _IONBF, 0);

  /* Minicom, screen, idf_monitor send CR when ENTER key is pressed. */
  esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM,
                                            ESP_LINE_ENDINGS_CR);
  /* Move caret to beginning of next line on '\n'. */
  esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM,
                                            ESP_LINE_ENDINGS_CRLF);

  /* Configure UART. Note REF_TICK is used so the baud rate remains correct
   * while APB frequency is changing in light sleep mode. */
  const uart_config_t uart_config = {
      .baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .source_clk = UART_SCLK_REF_TICK,
  };

  /* Install UART driver for interrupt-driven reads and writes. */
  ESP_ERROR_CHECK(
      uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));
  ESP_ERROR_CHECK(uart_param_config(CONFIG_ESP_CONSOLE_UART_NUM, &uart_config));

  /* Tell VFS to use UART driver. */
  esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

  /* Initialize console. */
  esp_console_config_t console_config = {
      .max_cmdline_length = MAX_INPUT_LENGTH,
  };

  ESP_ERROR_CHECK(esp_console_init(&console_config));

  /* Set command maximum length. */
  linenoiseSetMaxLineLen(console_config.max_cmdline_length);

  /* Don't return empty lines. */
  linenoiseAllowEmpty(false);
}
