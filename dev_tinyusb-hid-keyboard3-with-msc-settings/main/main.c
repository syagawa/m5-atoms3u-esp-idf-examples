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
// #include "app.h"

const char * initialDataStr = "{\"settings_mode\": \"storage\", \"color\": \"red\"}";
const char * versionStr = "tinyusb-msc-settings-1.2.0";

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
  initSettings(versionStr, initialDataStr);

  char * color = getSettingByKey("color");
  setButtonColor(color);

  while(1){
    if(buttonIsLongPressed){

      // char str[12];
      // snprintf(str, sizeof(str), "%d", pressedCount);
      // usb_hid_print_string("long");
      // usb_hid_print_string(str);

      if(pressedCount == 1){
        lightLed("yellow");
      }else if (pressedCount% 2 == 0 ) {
        lightLed("purple");
      }else{
        lightLed("green");
      }

      checkAndIncrementCount();
    }

    // if (tud_mounted()) {
    //     static bool send_hid_data = false;
    //     send_hid_data = !gpio_get_level(APP_BUTTON);
    //     if (send_hid_data) {
    //         usb_hid_print_string("User: ESP32-S3!\nPassword: Admin_123_|\\\n12345^~-=/?/.>,<_,______");

    //     }
    // }
    vTaskDelay(pdMS_TO_TICKS(100));
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
      lightLed("CYAN");
      ESP_LOGI(TAG, "normal");
      enterMain();
    }
  }

}
