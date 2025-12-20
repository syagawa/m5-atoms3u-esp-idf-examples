/* SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "iot_button.h"
#include "esp_private/usb_phy.h"
#include <stdint.h>
#include "tusb.h"
#include "esp_err.h"
#include "usb_descriptors.h"
#include "device/usbd.h"
#include <string.h>
#include "keyboard.h"


/* Button GPIO number */
#define EXAMPLE_BUTTON_NUM   4
#define EXAMPLE_BUTTON_UP    0
#define EXAMPLE_BUTTON_DOWN  11
#define EXAMPLE_BUTTON_LEFT  10
#define EXAMPLE_BUTTON_RIGHT 41


static int s_button_gpio[EXAMPLE_BUTTON_NUM] = {EXAMPLE_BUTTON_UP, EXAMPLE_BUTTON_DOWN, EXAMPLE_BUTTON_LEFT, EXAMPLE_BUTTON_RIGHT};
static button_handle_t s_button_handles[EXAMPLE_BUTTON_NUM] = {NULL};

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

static void tusb_device_task(void *arg)
{
    while (1) {
        tud_task();
    }
}

// Invoked when device is mounted
void tud_mount_cb(void)
{
    ESP_LOGI(TAG, "USB Mount");
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    ESP_LOGI(TAG, "USB Un-Mount");
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    ESP_LOGI(TAG, "USB Suspend");
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    ESP_LOGI(TAG, "USB Resume");
}

//--------------------------------------------------------------------+
// USB PHY config
//--------------------------------------------------------------------+
static void usb_phy_init(void)
{
    usb_phy_handle_t phy_hdl;
    // Configure USB PHY
    usb_phy_config_t phy_conf = {
        .controller = USB_PHY_CTRL_OTG,
        .otg_mode = USB_OTG_MODE_DEVICE,
    };
    phy_conf.target = USB_PHY_TARGET_INT;
    usb_new_phy(&phy_conf, &phy_hdl);
}

void initKeyButtonSettings(void){
    /* buttons init, buttons used to simulate keyboard or mouse events */
    button_config_t cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .active_level = 0,
        },
    };

    for (size_t i = 0; i < EXAMPLE_BUTTON_NUM; i++) {
        cfg.gpio_button_config.gpio_num = s_button_gpio[i];
        s_button_handles[i] = iot_button_create(&cfg);
        if (s_button_handles[i] == NULL) {
            ESP_LOGE(TAG, "Button io = %d created failed", s_button_gpio[i]);
            assert(0);
        } else {
            ESP_LOGI(TAG, "Button io = %d created", s_button_gpio[i]);
        }
    }


    button_cb_t button_cb = button_km_cb;

    ESP_LOGI(TAG, "Wait Mount through USB interface");

    /* register button callback, send HID report when click button */
    for (size_t i = 0; i < EXAMPLE_BUTTON_NUM; i++) {
        iot_button_register_cb(s_button_handles[i], BUTTON_SINGLE_CLICK, button_cb, NULL);
    }

}

void app_main(void)
{
    // switch esp usb phy to usb-otg
    usb_phy_init();
    tud_init(BOARD_TUD_RHPORT);
    xTaskCreate(tusb_device_task, "TinyUSB", 4096, NULL, 5, NULL);

    initKeyButtonSettings();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
