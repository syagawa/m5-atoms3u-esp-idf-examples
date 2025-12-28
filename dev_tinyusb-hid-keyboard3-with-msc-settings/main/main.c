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

#define waitingMS 1000
#define GPIOButtonNumber 41
#define MaxLength 10
char * defaultButtonColor = "red";
char * buttonColor = "";


#include "led.h"
#include "keyboard.h"
#include "button.h"
#include "storage.h"
// #include "app.h"

const char * initialDataStr = "{\"settings_mode\": \"storage\", \"color\": \"red\"}";
const char * versionStr = "tinyusb-msc-settings-1.2.0";

int keyIndex = 0;

char *keys[MaxLength];
int array_keys_count = 0;


int pressedCount = 0;
bool buttonIsLongPressed = false;
TickType_t lastIncrementTime = 0;

static void startCount() {
  buttonIsLongPressed = true;
  lastIncrementTime = xTaskGetTickCount();
  pressedCount = 1;
}

static void resetCount(){
  buttonIsLongPressed = false;
  pressedCount = 0;
}

static void checkAndIncrementCount() {
  TickType_t current = xTaskGetTickCount();
  if ((current - lastIncrementTime) >= pdMS_TO_TICKS(waitingMS)){
    pressedCount++;
    lastIncrementTime = xTaskGetTickCount();
  }
}

static void action1(void *arg,void *usr_data) {
    lightLed(buttonColor);

    // char *str = keys[keyIndex];
    // usb_hid_print_string(str);

    keyIndex++;
    if(keyIndex >= array_keys_count){
      keyIndex = 0;
    }

}
static void action2(void *arg, void *data) {
  resetCount();
}

static void action3(void *arg, void *data) {
  ESP_LOGI(TAG, "button_long_cb %d", pressedCount);
  startCount();
}



void enterSettingsMode(){
  lightLed("WHITE");
  startSettingsMode();
}

void enterMain(){

  singleClickAction = action1;
  pressUpAction = action2;
  longPressedAction = action3;



  initSettings(versionStr, initialDataStr);

  char * color = getSettingByKey("color");
  setButtonColor(color);





  cJSON *json_arr = getSettingByKey("keys");
  if (cJSON_IsArray(json_arr)) {
      int size = cJSON_GetArraySize(json_arr);
      for (int i = 0; i < size && i < MaxLength; i++) {
          cJSON *item = cJSON_GetArrayItem(json_arr, i);
          if (cJSON_IsString(item)) {
              keys[array_keys_count] = strdup(item->valuestring);
              array_keys_count++;
          }
      }
      // ESP_LOGI(TAG, "Successfully loaded %d keys from JSON", array_keys_count);
  }


  while(1){
    if(buttonIsLongPressed){

      // char str[12];
      // snprintf(str, sizeof(str), "%d", pressedCount);
      // usb_hid_print_string("long");
      // usb_hid_print_string(str);
      keyIndex = pressedCount;
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
