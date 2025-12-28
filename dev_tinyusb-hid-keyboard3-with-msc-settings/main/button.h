#ifndef BUTTON_H
#define BUTTON_H
#include "iot_button.h"
#include "storage.h"

int gpioBtnNum = 41;
int waitingMS = 1000;

int pushedBtnLong = 0;
int waitedMS = 0;
int buttonLongPressInited = 0;
int completedFirstWait = 0;

float brightness_test = 1.0;

char * defaultButtonColor = "red";
char * buttonColor = "";

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

static void button_long_cb(void *arg, void *data) {
  ESP_LOGI(TAG, "button_long_cb %d", pressedCount);
  startCount();
}

static void button_press_up_cb(void *arg, void *data)
{
  // ESP_LOGI(TAG, "button_press_up_cb");
  lightLed("blue");
  // usb_hid_print_string("up");
  // press_count = 0;
  resetCount();
}

static void button_single_click_cb(void *arg,void *usr_data)
{
    // ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");
    lightLed(buttonColor);
    // usb_hid_print_string("single");
    // usb_hid_print_string("User: ESP32-S3!\nPassword: Admin_123_|\\\n12345^~-=/?/.>,<_,______");

}


static void setButtonColor(char * color) {
  buttonColor = color;
}

char * getButtonColor(){
  if(strcmp(buttonColor, "") != 0) {
    return buttonColor;
  }
  return defaultButtonColor;
}




bool isButtonPressed(void){
  ESP_LOGI(TAG, "in isButtonPressed");
  return gpio_get_level(gpioBtnNum) == 0;
}



static void initButtonForKeyboard(void) {


  const gpio_config_t boot_button_config = {
      .pin_bit_mask = BIT64(gpioBtnNum),
      .mode = GPIO_MODE_INPUT,
      .intr_type = GPIO_INTR_DISABLE,
      .pull_up_en = true,
      .pull_down_en = false,
  };
  ESP_ERROR_CHECK(gpio_config(&boot_button_config));

  ESP_LOGI(TAG, "USB initialization");
  const tinyusb_config_t tusb_cfg = {
      .device_descriptor = NULL,
      .string_descriptor = hid_string_descriptor,
      .string_descriptor_count = sizeof(hid_string_descriptor) / sizeof(hid_string_descriptor[0]),
      .external_phy = false,
      .configuration_descriptor = hid_configuration_descriptor,
  };

  ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
  ESP_LOGI(TAG, "USB initialization DONE");


  // create gpio button
  button_config_t gpio_btn_cfg = {
    .type = BUTTON_TYPE_GPIO,
    // .long_press_ticks = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
    // .long_press_ticks = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
    // .short_press_ticks = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
    .long_press_time = waitingMS,
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
  // iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, button_km_cb,NULL);
  iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, button_single_click_cb,NULL);
  iot_button_register_cb(gpio_btn, BUTTON_LONG_PRESS_START, button_long_cb,NULL);
  iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, button_press_up_cb,NULL);
}


#endif // BUTTON_H
