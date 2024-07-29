#include "Buzzer.h"
#include <driver/ledc.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/idf_additions.h>

const char *const Buzzer::TAG = "Buzzer";

Buzzer::Buzzer()
    : TIMER_NUM(LEDC_TIMER_0), CHANNEL(LEDC_CHANNEL_0),
      SPEED_MODE(LEDC_LOW_SPEED_MODE), DUTY(4096), FREQ_HZ(440), GPIO_NUM(26),
      timer_config(), channel_config(), alarm_is_on(false) {
  timer_config = {
      .speed_mode = SPEED_MODE,
      .duty_resolution = LEDC_TIMER_13_BIT,
      .timer_num = TIMER_NUM,
      .freq_hz = FREQ_HZ,
      .clk_cfg = LEDC_AUTO_CLK,
  };
  ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

  channel_config = {
      .gpio_num = GPIO_NUM,
      .speed_mode = SPEED_MODE,
      .channel = CHANNEL,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = TIMER_NUM,
      .duty = 0,
      .hpoint = 0,
  };
  ESP_ERROR_CHECK(ledc_channel_config(&channel_config));
};

Buzzer::~Buzzer() { ESP_LOGI(TAG, "Destroy."); }

void Buzzer::alarm_task(void *pvParameters) {
  Buzzer *self = static_cast<Buzzer *>(pvParameters);

  while (self->alarm_is_on) {
    ESP_ERROR_CHECK(ledc_set_duty(self->SPEED_MODE, self->CHANNEL, self->DUTY));
    ESP_ERROR_CHECK(ledc_update_duty(self->SPEED_MODE, self->CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(500));

    ESP_ERROR_CHECK(ledc_set_duty(self->SPEED_MODE, self->CHANNEL, 0));
    ESP_ERROR_CHECK(ledc_update_duty(self->SPEED_MODE, self->CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  vTaskDelete(NULL);
}

void Buzzer::start_alarm() {
  alarm_is_on = true;
  xTaskCreate(alarm_task, "alarm", 2048, this, 5, NULL);
  ESP_LOGI(TAG, "Started alarm.");
}

void Buzzer::stop_alarm() {
  alarm_is_on = false;
  ESP_LOGI(TAG, "Stopped alarm.");
}
