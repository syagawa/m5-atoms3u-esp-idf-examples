
#ifndef STORAGE_H
#define STORAGE_H
#include "tinyusb.h"
#include "tusb_msc_storage.h"
#include "tusb_cdc_acm.h"

#include "esp_log.h"
#include "sdkconfig.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "cJSON.h"


#define BASE_PATH "/usb" // base path to mount the partition

static const char *TAG = "tusb-hid-keyboard-msc-settings";
static uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];

const char *directory = "/usb/esp";
const char *file_path = "/usb/esp/settings.txt";

const char *file_path_version = "/usb/esp/version.txt";

static const char * readmeStr = "Reset Settings: Delete settings.txt and remove this USB from PC.";
const char *file_path_readme = "/usb/esp/readme.txt";


void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    /* initialization */
    size_t rx_size = 0;

    /* read */
    esp_err_t ret = tinyusb_cdcacm_read(itf, buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Data from channel %d:", itf);
        ESP_LOG_BUFFER_HEXDUMP(TAG, buf, rx_size, ESP_LOG_INFO);
    } else {
        ESP_LOGE(TAG, "Read error");
    }

    /* write back */
    tinyusb_cdcacm_write_queue(itf, buf, rx_size);
    tinyusb_cdcacm_write_flush(itf, 0);
}

void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    int dtr = event->line_state_changed_data.dtr;
    int rts = event->line_state_changed_data.rts;
    ESP_LOGI(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
}

static bool file_exists(const char *file_path)
{
    struct stat buffer;
    return stat(file_path, &buffer) == 0;
}


static esp_err_t storage_init_spiflash(wl_handle_t *wl_handle)
{
    ESP_LOGI(TAG, "Initializing wear levelling");

    const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
    if (data_partition == NULL) {
        ESP_LOGE(TAG, "Failed to find FATFS partition. Check the partition table.");
        return ESP_ERR_NOT_FOUND;
    }

    return wl_mount(data_partition, wl_handle);
}


static void removeFiles(void){
    fclose(file_path);
    remove(file_path);
}

// can not reset settings
static void resetSettings(char * initialDataStr) {

    tusb_cdc_acm_deinit(1);

    struct stat s = {0};
    bool directory_exists = stat(directory, &s) == 0;
    if (!directory_exists) {
        if (mkdir(directory, 0775) != 0) {
            ESP_LOGE(TAG, "mkdir failed with errno: %s", strerror(errno));
            return;
        }
    }

    
    removeFiles();

    ESP_LOGI(TAG, "Creating file");
    FILE *f1 = fopen(file_path, "w");
    if (f1 == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        // return;
    }
    // fprintf(f, "Hello World!\n");
    fprintf(f1, initialDataStr);
    fclose(f1);

}

void initSettings(char * version, char * initialDataStr){

    wl_handle_t wl_handle = WL_INVALID_HANDLE;
    ESP_ERROR_CHECK(storage_init_spiflash(&wl_handle));

    const tinyusb_msc_spiflash_config_t config_spi = {
        .wl_handle = wl_handle
    };
    ESP_ERROR_CHECK(tinyusb_msc_storage_init_spiflash(&config_spi));
    ESP_ERROR_CHECK(tinyusb_msc_storage_mount(BASE_PATH));

    struct stat s = {0};
    bool directory_exists = stat(directory, &s) == 0;
    if (!directory_exists) {
        if (mkdir(directory, 0775) != 0) {
            ESP_LOGE(TAG, "mkdir failed with errno: %s", strerror(errno));
        }
    }

    if (!file_exists(file_path)) {
        ESP_LOGI(TAG, "Creating file");
        FILE *f = fopen(file_path, "w");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for writing");
        }
        fprintf(f, initialDataStr);
        fclose(f);
    }

    FILE *f1 = fopen(file_path_version, "w");
    if (f1 == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
    }
    fprintf(f1, version);
    fclose(f1);

    FILE *f2 = fopen(file_path_readme, "w");
    if (f2 == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
    }
    fprintf(f2, readmeStr);
    fclose(f2);
}

cJSON * getSettings(){
    FILE *f;
    ESP_LOGI(TAG, "Reading file");
    f = fopen(file_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    char * str = strdup(line);
    // strdupを使用している場合、必要に応じてfree(str);を呼び出してメモリを解放する必要があります

    ESP_LOGI(TAG, "json_str '%s'", str);
    cJSON * obj = cJSON_Parse(str);
    free(str);

    return obj;

}

char * getSettingByKey(char * targetkey){

    cJSON * obj = getSettings();
    char * value = "";
    if(obj != NULL){
      // Iteratively check for existing keys
      cJSON *currentItem = obj->child;
      while (currentItem != NULL) {
          const char *key = currentItem->string;
          cJSON *value = currentItem;
          if (cJSON_IsString(value) && (value->valuestring != NULL)) {
              ESP_LOGI(TAG, "Key: %s, Value: %s\n", key, value->valuestring);
          } else {
              ESP_LOGE(TAG, "Error getting value for key: %s\n", key);
          }
          currentItem = currentItem->next;
      }

      cJSON *target_elm = cJSON_GetObjectItemCaseSensitive(obj, targetkey);


      if(target_elm){
        value = target_elm->valuestring;
      }

    }

    return value;


}

const char* msc_string_descriptor[5] = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    "TinyUSB",             // 1: Manufacturer
    "TinyUSB Device",      // 2: Product
    "123456",              // 3: Serials, should use chip ID
    "Example MSC",  // 4: HID
};

static const tusb_desc_device_t test_device_descriptor = {
    .bLength = sizeof(test_device_descriptor),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0x303A, // This is Espressif VID. This needs to be changed according to Users / Customers
    .idProduct = 0x4002,
    .bcdDevice = 0x100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01
};


tinyusb_config_t getTusbConfigStorage() {
    const tinyusb_config_t tusb_cfgStorage = {
        .device_descriptor = &test_device_descriptor,
        // .device_descriptor = NULL,
        // .string_descriptor = NULL,
        .string_descriptor = msc_string_descriptor,
        // .string_descriptor_count = 0,
        .string_descriptor_count = sizeof(msc_string_descriptor) / sizeof(msc_string_descriptor[0]),
        .external_phy = false,
        .configuration_descriptor = NULL,
    };
    return tusb_cfgStorage;
}

void startSettingsMode(){

    cJSON * obj = getSettings();

    char * settings_mode = "storage";//stprage, web
    if(obj != NULL){
      // Iteratively check for existing keys
      cJSON *currentItem = obj->child;
      while (currentItem != NULL) {
          const char *key = currentItem->string;
          cJSON *value = currentItem;
          if (cJSON_IsString(value) && (value->valuestring != NULL)) {
              ESP_LOGI(TAG, "Key: %s, Value: %s\n", key, value->valuestring);
          } else {
              ESP_LOGE(TAG, "Error getting value for key: %s\n", key);
          }
          currentItem = currentItem->next;
      }

      cJSON *settings_mode_elm = cJSON_GetObjectItemCaseSensitive(obj, "settings_mode");

      if(settings_mode_elm){
        settings_mode = settings_mode_elm->valuestring;
      }

    }

    ESP_LOGI(TAG, "Settings Mode is %s\n", settings_mode);
    
    if(strcmp("storage", settings_mode) == 0){
      showColorWithBrightness("YELLOW", 0.1);

      ESP_LOGI(TAG, "USB Composite initialization");
      const tinyusb_config_t tusb_cfgStorage = getTusbConfigStorage();
    //   const tinyusb_config_t tusb_cfgStorage = {
    //       .device_descriptor = NULL,
    //       .string_descriptor = NULL,
    //       .string_descriptor_count = 0,
    //       .external_phy = false,
    //       .configuration_descriptor = NULL,
    //   };
      ESP_LOGI(TAG, "USB Composite initialization1");
      ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfgStorage));
      ESP_LOGI(TAG, "USB Composite initialization2");

      showColorWithBrightness("RED", 0.1);

      tinyusb_config_cdcacm_t acm_cfg = {
          .usb_dev = TINYUSB_USBDEV_0,
          .cdc_port = TINYUSB_CDC_ACM_0,
          .rx_unread_buf_sz = 64,
          .callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
          .callback_rx_wanted_char = NULL,
          .callback_line_state_changed = NULL,
          .callback_line_coding_changed = NULL
      };
      ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
      showColorWithBrightness("green", 0.1);

      ESP_LOGI(TAG, "USB Composite initialization3");
      /* the second way to register a callback */
      ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                          TINYUSB_CDC_ACM_0,
                          CDC_EVENT_LINE_STATE_CHANGED,
                          &tinyusb_cdc_line_state_changed_callback));

      ESP_LOGI(TAG, "USB Composite initialization DONE");



    }else if(strcmp("web", settings_mode) == 0){
      showColorWithBrightness("BLUE", 0.1);
    }else{
      showColorWithBrightness("RED", 0.1);
    }

    cJSON_Delete(obj);

}

#endif // STORAGE_H