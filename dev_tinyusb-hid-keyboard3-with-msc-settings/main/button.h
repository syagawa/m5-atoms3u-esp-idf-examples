#ifndef BUTTON_H
#define BUTTON_H
#include "iot_button.h"
#include "storage.h"

int gpioBtnNum = 41;
int bootmode = 0;// 0 Main Mode
                 // 1. Settings Mode(USB Flash, Web, other)
int pushedBtnLong = 0;
int waitedMS = 0;
int buttonLongPressInited = 0;
int completedFirstWait = 0;

float brightness_test = 1.0;

char * defaultButtonColor = "red";
char * buttonColor = "";

void (*singleClickAction)(void);
void (*doubleClickAction)(void);
void (*longPressedAction)(void);


// static void setButtonLongPressInited(){
//   buttonLongPressInited = 1;
// }

// static void setBootModeSettings(){
//   bootmode = 1;
// }


// static void setCompletedFirstWait(){
//   completedFirstWait = 1;
// }

static void setButtonColor(char * color) {
  buttonColor = color;
}

char * getButtonColor(){
  if(strcmp(buttonColor, "") != 0) {
    return buttonColor;
  }
  return defaultButtonColor;
}


// static void button_single_click_cb(void *arg,void *usr_data)
// {
//     ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");
//     ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");

//     ESP_LOGI(TAG, "pushedBtnLong %d", pushedBtnLong);
//     ESP_LOGI(TAG, "waitedMS %d", waitedMS);
//     ESP_LOGI(TAG, "buttonLongPressInited %d", buttonLongPressInited);

//     int a = iot_button_get_repeat((button_handle_t)arg);
//     ESP_LOGI(TAG, "BUTTON_SINGLE_CLICKaaa %d", a);
//     if(isBootModeMain() && completedFirstWait == 1 && singleClickAction != NULL){
//       singleClickAction();
//     }
// }

// static void button_long_press_cb(void *arg,void *usr_data){
//     ESP_LOGI(TAG, "BUTTON_LONG_PRESS_START_1");
//     if(buttonLongPressInited == 0){
//       pushedBtnLong = 1;
//     }
//     if(isBootModeMain() && completedFirstWait == 1 && longPressedAction != NULL){
//       longPressedAction();
//     }
// }

// static void button_double_click_cb(void *arg,void *usr_data){

//     int num = iot_button_get_repeat((button_handle_t)arg);
//     ESP_LOGI(TAG, "Click count: %d", num);

//     // if(num == 2){
//     //   if(bootmode == 1){
//     //     esp_restart();
//     //   }
//     //   if(isBootModeMain() && completedFirstWait == 1 && doubleClickAction != NULL){
//     //     doubleClickAction();
//     //   }
//     // }P32-S3!
// }


bool isButtonPressed(void){
  ESP_LOGI(TAG, "in isButtonPressed");
  return gpio_get_level(gpioBtnNum) == 0;
}


// static void initButton(void) {
//   // create gpio button
//   button_config_t gpio_btn_cfg = {
//     .type = BUTTON_TYPE_GPIO,
//     // .long_press_ticks = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
//     // .long_press_ticks = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
//     // .short_press_ticks = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
//     .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
//     .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
//     .gpio_button_config = {
//         .gpio_num = gpioBtnNum,
//         .active_level = 0,
//     },
//   };

//   button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);

//   if (gpio_btn == NULL) {
//     ESP_LOGE(TAG, "Button create failed");
//   }
//   // iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, button_km_cb,NULL);
//   // iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, button_single_click_cb,NULL);
//   iot_button_register_cb(gpio_btn, BUTTON_LONG_PRESS_START, button_long_press_cb, NULL);
//   iot_button_register_cb(gpio_btn, BUTTON_DOUBLE_CLICK, button_double_click_cb, NULL);
// }

static void initButtonForKeyboard(void) {
  // create gpio button
  button_config_t gpio_btn_cfg = {
    .type = BUTTON_TYPE_GPIO,
    // .long_press_ticks = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
    // .long_press_ticks = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
    // .short_press_ticks = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
    .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
    .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
    .gpio_button_config = {
        .gpio_num = gpioBtnNum,
        .active_level = 0,
    },
  };

  button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);

  if (gpio_btn == NULL) {
    ESP_LOGE(TAG, "Button create failed");
  }
  iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, button_km_cb,NULL);
  // iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, button_single_click_cb,NULL);
}


#endif // BUTTON_H
