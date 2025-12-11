/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include "esp_partition.h"
#include "esp_check.h"
#include "tinyusb.h"
#include "tusb_msc_storage.h"
#include "tusb_cdc_acm.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

// #include "iot_button.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>

// #include "jsmn.h"
#include "cJSON.h"

#include "led.h"
#include "button.h"
#include "storage.h"
#include "app.h"


#include "esp_system.h"
#include "esp_private/startup_internal.h"

static esp_reset_reason_t saved_reset_reason; 

RTC_DATA_ATTR static int boot_mode = 0;

static void early_hook(void)
{
    saved_reset_reason = esp_reset_reason();
    // ここは app_main より前。ログやフラグ保存などが可能。
}

void firstWait(int sec){

  int delayMS = 10;
  int ms = 1000;
  int waitMS = sec * ms;

  TickType_t waitStartTime = xTaskGetTickCount();

  ESP_LOGI(TAG, "firstWait0 %ld", xTaskGetTickCount());

  while((xTaskGetTickCount() - waitStartTime) <= pdMS_TO_TICKS(waitMS)){

    if(pushedBtnLong == 1){
      setButtonLongPressInited();
      setBootModeSettings();
      ESP_LOGI(TAG, "firstWait1 %ld", xTaskGetTickCount());
      break;
    }
    vTaskDelay(delayMS / portTICK_PERIOD_MS);

  }

  ESP_LOGI(TAG, "firstWait2 %ld", xTaskGetTickCount());

  setCompletedFirstWait();
}

void onSingleClick() {
  esp_restart();
}


void app_main(void){
  ESP_LOGI(TAG, "Reset saved_reset_reason = %d", saved_reset_reason);
  esp_reset_reason_t reason = esp_reset_reason();

  // 通常起動だと 11 esp_restartだとreset reason が3になる
  ESP_LOGI(TAG, "Reset reason = %d", reason);

  ESP_LOGI(TAG, "Reset boot_mode = %d", boot_mode);

  singleClickAction = onSingleClick;

  initButton();
  initLed();

  boot_mode = 1;

  if(reason == 11){
    lightLed("blue");
  }else if(reason == 3){
    lightLed("green");
  }else{
    lightLed("white");
  }

}
