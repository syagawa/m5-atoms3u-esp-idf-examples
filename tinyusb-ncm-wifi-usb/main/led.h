#include "led_strip.h"

#define BLINK_GPIO 35

static const char *TAG_LED = "example_main_led";

static uint8_t s_led_state = 0;
static led_strip_handle_t led_strip;

void showColor(char* color) {
  if(strcmp("BLACK", color) == 0 || strcmp("black", color) == 0){
    led_strip_set_pixel(led_strip, 0, 0, 0, 0 );
  }else if(strcmp("NAVY", color) == 0 || strcmp("navy", color) == 0){
    led_strip_set_pixel(led_strip, 0, 0, 0, 128 );
  }else if(strcmp("DARKGREEN", color) == 0 || strcmp("darkgreen", color) == 0){
    led_strip_set_pixel(led_strip, 0, 0, 128, 0 );
  }else if(strcmp("DARKCYAN", color) == 0 || strcmp("darkcyan", color) == 0){
    led_strip_set_pixel(led_strip, 0, 0, 128, 128 );
  }else if(strcmp("MAROON", color) == 0 || strcmp("maroon", color) == 0){
    led_strip_set_pixel(led_strip, 0, 128, 0, 0 );
  }else if(strcmp("PURPLE", color) == 0 || strcmp("purple", color) == 0){
    led_strip_set_pixel(led_strip, 0, 128, 0, 128 );
  }else if(strcmp("OLIVE", color) == 0 || strcmp("olive", color) == 0){
    led_strip_set_pixel(led_strip, 0, 128, 128, 0 );
  }else if(strcmp("LIGHTGREY", color) == 0 || strcmp("lightgrey", color) == 0){
    led_strip_set_pixel(led_strip, 0, 211, 211, 211 );
  }else if(strcmp("LIGHTGRAY", color) == 0 || strcmp("lightgray", color) == 0){
    led_strip_set_pixel(led_strip, 0, 211, 211, 211 );
  }else if(strcmp("DARKGREY", color) == 0 || strcmp("darkgrey", color) == 0){
    led_strip_set_pixel(led_strip, 0, 128, 128, 128 );
  }else if(strcmp("DARKGRAY", color) == 0 || strcmp("darkgray", color) == 0){
    led_strip_set_pixel(led_strip, 0, 128, 128, 128 );
  }else if(strcmp("BLUE", color) == 0 || strcmp("blue", color) == 0){
    led_strip_set_pixel(led_strip, 0, 0, 0, 255 );
  }else if(strcmp("GREEN", color) == 0 || strcmp("green", color) == 0){
    led_strip_set_pixel(led_strip, 0, 0, 255, 0 );
  }else if(strcmp("CYAN", color) == 0 || strcmp("cyan", color) == 0){
    led_strip_set_pixel(led_strip, 0, 0, 255, 255 );
  }else if(strcmp("RED", color) == 0 || strcmp("red", color) == 0){
    led_strip_set_pixel(led_strip, 0, 255, 0, 0 );
  }else if(strcmp("MAGENTA", color) == 0 || strcmp("magenta", color) == 0){
    led_strip_set_pixel(led_strip, 0, 255, 0, 255 );
  }else if(strcmp("YELLOW", color) == 0 || strcmp("yellow", color) == 0){
    led_strip_set_pixel(led_strip, 0, 255, 255, 0 );
  }else if(strcmp("WHITE", color) == 0 || strcmp("white", color) == 0){
    led_strip_set_pixel(led_strip, 0, 255, 255, 255 );
  }else if(strcmp("ORANGE", color) == 0 || strcmp("orange", color) == 0){
    led_strip_set_pixel(led_strip, 0, 255, 180, 0 );
  }else if(strcmp("GREENYELLOW", color) == 0 || strcmp("greenyellow", color) == 0){
    led_strip_set_pixel(led_strip, 0, 180, 255, 0 );
  }else if(strcmp("PINK", color) == 0 || strcmp("pink", color) == 0){
    led_strip_set_pixel(led_strip, 0, 255, 192, 203 );
  }else if(strcmp("BROWN", color) == 0 || strcmp("brown", color) == 0){
    led_strip_set_pixel(led_strip, 0, 150, 75, 0 );
  }else if(strcmp("GOLD", color) == 0 || strcmp("gold", color) == 0){
    led_strip_set_pixel(led_strip, 0, 255, 215, 0 );
  }else if(strcmp("SILVER", color) == 0 || strcmp("silver", color) == 0){
    led_strip_set_pixel(led_strip, 0, 192, 192, 192 );
  }else if(strcmp("SKYBLUE", color) == 0 || strcmp("skyblue", color) == 0){
    led_strip_set_pixel(led_strip, 0, 135, 206, 235 );
  }else if(strcmp("VIOLET", color) == 0 || strcmp("violet", color) == 0){
    led_strip_set_pixel(led_strip, 0, 180, 46, 226 );
  }else{
    led_strip_set_pixel(led_strip, 0, 16, 16, 16 );
  }
  led_strip_refresh(led_strip);
}

static void blink_led(char* color)
{
    if (s_led_state) {
        showColor(color);
        led_strip_refresh(led_strip);
    } else {
        led_strip_clear(led_strip);
    }
}

static void lightLed(char* color) {
    led_strip_clear(led_strip);
    showColor(color);
    led_strip_refresh(led_strip);
}

static void initLed(void)
{
    ESP_LOGI(TAG_LED, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
        // .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        // .led_model = LED_MODEL_WS2812,
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags = {
          .with_dma = false,
        },
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}