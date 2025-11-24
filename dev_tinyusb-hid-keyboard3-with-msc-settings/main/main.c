// 下記コードがリセットかどうかでMSC/HIDを切り替えるサンプルコード
#include "esp_system.h"
#include "esp_log.h"
#include "tinyusb.h"
#include "tusb_msc_storage.h"

static const char *TAG = "usb_mode";

void start_hid_keyboard(void)
{
    ESP_LOGI(TAG, "Starting HID Keyboard mode...");
    // tinyUSB HID 初期化処理
    tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false,
    };
    tinyusb_driver_install(&tusb_cfg);

    // HIDキーボード用の記述子設定など…
}

void start_msc(void)
{
    ESP_LOGI(TAG, "Starting MSC mode...");
    // tinyUSB MSC 初期化処理
    const tinyusb_msc_storage_config_t msc_cfg = {
        .block_count = 2048,
        .block_size = 512,
        .read_cb  = my_msc_read_cb,
        .write_cb = my_msc_write_cb,
    };

    tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false,
    };

    tinyusb_driver_install(&tusb_cfg);
    tinyusb_msc_storage_init(&msc_cfg);
}

void app_main(void)
{
    esp_reset_reason_t reason = esp_reset_reason();
    ESP_LOGI(TAG, "Reset reason = %d", reason);

    // ------------------------------------------
    //  Reset理由で USB モードを切り替える
    // ------------------------------------------
    if (reason == ESP_RST_EXT) {
        // ENピンによるリセット → MSC
        start_msc();
    } else {
        // 通常の再起動・電源投入 → HID
        start_hid_keyboard();
    }
}

