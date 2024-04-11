#ifndef APP_H
#define APP_H

#include "storage.h"
#include "button.h"


const char *  versionStr = "tinyusb-ncm-wifi-usb-with-msc-settings-1.1.0";

static void buttonAction1(){
  char * color = getButtonColor();
  showColorWithBrightness(color, brightness_test);
  brightness_test = brightness_test - 0.1;
  if(brightness_test < 0){
    brightness_test = 1.0;
  }
}

static void buttonAction2(){
    ESP_LOGI(TAG, "double clicked!");
}

void setApp(){
    // write initialize for app
    char * color = getSettingByKey("color");
    setButtonColor(color);
    ESP_LOGI(TAG, "Color: %s!", color);

    singleClickAction = buttonAction1;
    doubleClickAction = buttonAction2;
}

void appInLoop(){
    // write logic in loop
}

#endif // APP_H