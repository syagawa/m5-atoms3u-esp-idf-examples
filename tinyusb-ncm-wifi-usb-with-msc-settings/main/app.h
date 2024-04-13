#ifndef APP_H
#define APP_H

#include "storage.h"
#include "button.h"

#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_mac.h"

#include "esp_wifi.h"
#include "esp_private/wifi.h"

#include "tinyusb_net.h"

const char * initialDataStr = "{\"settings_mode\": \"storage\", \"ssid\": \"ssid11\", \"ps\": \"ps11\"}";
const char *  versionStr = "tinyusb-ncm-wifi-usb-with-msc-settings-1.0.1";

static esp_err_t usb_recv_callback(void *buffer, uint16_t len, void *ctx)
{
    bool *is_wifi_connected = ctx;
    if (*is_wifi_connected) {
        esp_wifi_internal_tx(ESP_IF_WIFI_STA, buffer, len);
    }
    return ESP_OK;
}

static void wifi_pkt_free(void *eb, void *ctx)
{
    esp_wifi_internal_free_rx_buffer(eb);
}

static esp_err_t pkt_wifi2usb(void *buffer, uint16_t len, void *eb)
{
    if (tinyusb_net_send_sync(buffer, len, eb, portMAX_DELAY) != ESP_OK) {
        esp_wifi_internal_free_rx_buffer(eb);
    }
    return ESP_OK;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    bool *is_connected = arg;

    showColor("white");

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WiFi STA disconnected");
        *is_connected = false;
        showColor("red");
        esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL);
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "WiFi STA connected");
        showColor("green");
        esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, pkt_wifi2usb);
        *is_connected = true;
    }
}

static esp_err_t start_wifi(bool *is_connected, char *ssid, char *ps){
    ESP_RETURN_ON_ERROR(esp_event_loop_create_default(), TAG, "Cannot initialize event loop");

    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(esp_wifi_init(&wifi_cfg), TAG, "Failed to initialize WiFi library");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, is_connected),
                        TAG, "Failed to register handler for wifi events");
    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), TAG, "Failed to set WiFi station mode");
    ESP_RETURN_ON_ERROR(esp_wifi_start(), TAG, "Failed to start WiFi library");

    int len_ssid = strlen(ssid);
    char array_ssid[len_ssid + 1];
    strcpy(array_ssid, ssid);

    int len_ps = strlen(ps);
    char array_ps[len_ps + 1];
    strcpy(array_ps, ps);

    // wifi_config_t wifi_config = {
    //     .sta = {
    //         .ssid = a,
    //         .password = b,
    //     },
    // };


    wifi_config_t wifi_config = {0};  // 全てのメンバをゼロで初期化

    // SSIDとパスワードを配列にコピー
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = 0;  // 念の為のヌル終端

    strncpy((char *)wifi_config.sta.password, ps, sizeof(wifi_config.sta.password));
    wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = 0;



    ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_STA, &wifi_config), TAG, "Failed to set WiFi config");
    return esp_wifi_connect();
}


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

static void buttonAction3(){
  ESP_LOGI(TAG, "long pressed!");
}

void setApp()
{

    char * color = getSettingByKey("color");
    setButtonColor(color);
    ESP_LOGI(TAG, "Color: %s!", color);

    singleClickAction = buttonAction1;
    doubleClickAction = buttonAction2;
    longPressedAction = buttonAction3;

    char * ssid = getSettingByKey("ssid");
    char * ps = getSettingByKey("ps");

    static bool s_is_wifi_connected = false;    // needs to be static as it's used after we exit app_main()

    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "USB NCM device initialization");
    const tinyusb_config_t tusb_cfg = {
        .external_phy = false,
    };
    ESP_GOTO_ON_ERROR(tinyusb_driver_install(&tusb_cfg), err, TAG, "Failed to install TinyUSB driver");

    tinyusb_net_config_t net_config = {
        .on_recv_callback = usb_recv_callback,
        .free_tx_buffer = wifi_pkt_free,
        .user_context = &s_is_wifi_connected
    };
    esp_read_mac(net_config.mac_addr, ESP_MAC_WIFI_STA);
    uint8_t *mac = net_config.mac_addr;
    ESP_LOGI(TAG, "Network interface HW address: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    ESP_GOTO_ON_ERROR(tinyusb_net_init(TINYUSB_USBDEV_0, &net_config), err, TAG, "Failed to initialize TinyUSB NCM device class");

    ESP_LOGI(TAG, "WiFi initialization");
    ESP_GOTO_ON_ERROR(start_wifi(&s_is_wifi_connected, ssid, ps), err, TAG, "Failed to init and start WiFi");

    ESP_LOGI(TAG, "USB NCM and WiFi initialized and started");
    return;

err:
    ESP_LOGE(TAG, "USB-WiFi bridge example failed!");
}



void appInLoop(){
    // write logic in loop
}

#endif // APP_H