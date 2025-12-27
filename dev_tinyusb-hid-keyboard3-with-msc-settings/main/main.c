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
#include "keyboard.h"
#include "button.h"
#include "storage.h"
#include "app.h"



// void firstWait(int sec){

//   int delayMS = 10;
//   int ms = 1000;
//   int waitMS = sec * ms;

//   TickType_t waitStartTime = xTaskGetTickCount();

//   ESP_LOGI(TAG, "firstWait0 %ld", xTaskGetTickCount());

//   while((xTaskGetTickCount() - waitStartTime) <= pdMS_TO_TICKS(waitMS)){

//     if(pushedBtnLong == 1){
//       setButtonLongPressInited();
//       setBootModeSettings();
//       ESP_LOGI(TAG, "firstWait1 %ld", xTaskGetTickCount());
//       break;
//     }
//     vTaskDelay(delayMS / portTICK_PERIOD_MS);

//   }

//   ESP_LOGI(TAG, "firstWait2 %ld", xTaskGetTickCount());

//   setCompletedFirstWait();
// }



void enterSettingsMode(){
  lightLed("WHITE");
  startSettingsMode();
}

void enterMain(){
  // setApp();
  while(1){
    appInLoop();
    vTaskDelay(1);
  }
}

void app_main(void){

  esp_reset_reason_t reason = esp_reset_reason();

  // initButton();

  if(reason == 3){
    ESP_LOGI(TAG, "restarted esp");
    initLed();
    lightLed("green");
    initSettings(versionStr, initialDataStr);
    ESP_LOGI(TAG, "after initSettings");
    enterSettingsMode();
    ESP_LOGI(TAG, "after enterSettingsMode");
    // settings mode
  }else{
    // initButton();
    if(isButtonPressed()){
      // lightLed("orange");
      ESP_LOGI(TAG, "pressed1");
      esp_restart();
    }else{
      initButtonForKeyboard();
      initLed();
      lightLed("red");
      ESP_LOGI(TAG, "normal");
      enterMain();
    }
  }

}
