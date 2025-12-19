
#define TAG "HID Example1"
static bool s_keyboard_pressed = false;

void tinyusb_hid_keyboard_report(uint8_t keycode[], int shift)
{
    // ESP_LOGD(TAG, "keycode = %u %u %u %u %u %u", keycode[0], keycode[1], keycode[2], keycode[3], keycode[4], keycode[5]);

    // Remote wakeup
    if (tud_suspended()) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    } else {
        // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
        // skip if hid is not ready yet
        if (!tud_hid_ready()) {
            ESP_LOGW(TAG, "tinyusb not ready %s %d", __func__, __LINE__);
            return;
        }

        uint8_t _keycode[6] = { 0 };
        _keycode[0] = keycode[0];
        _keycode[1] = keycode[1];
        _keycode[2] = keycode[2];
        _keycode[3] = keycode[3];
        _keycode[4] = keycode[4];
        _keycode[5] = keycode[5];

        if(shift == 1){
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, KEYBOARD_MODIFIER_LEFTSHIFT, _keycode);
        }else{
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, _keycode);
        }
        s_keyboard_pressed = true;
    }
}
const char* keyboard_layout_mode = "jis"; //jis, us
static void ascii_to_hid_with_modifier(char c, uint8_t *keycode, uint8_t *modifier) {
    *modifier = 0;
    *keycode = 0;

     // アルファベット小文字
     if (c >= 'a' && c <= 'z') {
         *keycode = (c - 'a') + HID_KEY_A;
     // アルファベット大文字
    } else if (c >= 'A' && c <= 'Z') {
         *keycode = (c - 'A') + HID_KEY_A;
         *modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
     // 数字
    } else if (c >= '1' && c <= '9') {
         *keycode = (c - '1') + HID_KEY_1;
    } else if (c == '0') {
         *keycode = HID_KEY_0;
    // 記号と特殊キー (US配列基準)
    } else {
        if(strcmp(keyboard_layout_mode, "us") == 0){
            switch (c) {
                 case ' ':  *keycode = HID_KEY_SPACE; break;
                 case '\n': *keycode = HID_KEY_ENTER; break;
                 case '\t': *keycode = HID_KEY_TAB; break;
                 case '-':  *keycode = HID_KEY_MINUS; break;
                 case '=':  *keycode = HID_KEY_EQUAL; break;
                 case '[':  *keycode = HID_KEY_BRACKET_LEFT; break;
                 case ']':  *keycode = HID_KEY_BRACKET_RIGHT; break;
                 case '\\': *keycode = HID_KEY_BACKSLASH; break;
                 case ';':  *keycode = HID_KEY_SEMICOLON; break;
                 case '\'': *keycode = HID_KEY_APOSTROPHE; break;
                 case '`':  *keycode = HID_KEY_GRAVE; break;
                 case ',':  *keycode = HID_KEY_COMMA; break;
                 case '.':  *keycode = HID_KEY_PERIOD; break;
                 case '/':  *keycode = HID_KEY_SLASH; break;
    
                 // Shiftが必要な記号
                 case '!':  *keycode = HID_KEY_1; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '@':  *keycode = HID_KEY_2; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '#':  *keycode = HID_KEY_3; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '$':  *keycode = HID_KEY_4; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '%':  *keycode = HID_KEY_5; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '^':  *keycode = HID_KEY_6; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '&':  *keycode = HID_KEY_7; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '*':  *keycode = HID_KEY_8; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '(':  *keycode = HID_KEY_9; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case ')':  *keycode = HID_KEY_0; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '_':  *keycode = HID_KEY_MINUS; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '+':  *keycode = HID_KEY_EQUAL; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '{':  *keycode = HID_KEY_BRACKET_LEFT; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '}':  *keycode = HID_KEY_BRACKET_RIGHT; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '|':  *keycode = HID_KEY_BACKSLASH; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case ':':  *keycode = HID_KEY_SEMICOLON; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '\"': *keycode = HID_KEY_APOSTROPHE; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '~':  *keycode = HID_KEY_GRAVE; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '<':  *keycode = HID_KEY_COMMA; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '>':  *keycode = HID_KEY_PERIOD; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                 case '?':  *keycode = HID_KEY_SLASH; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
    
                 default:   *keycode = 0; break; // 未対応
            }
            

        }else{
            switch (c) {
                case ' ':  *keycode = HID_KEY_SPACE; break;
                case '\n': *keycode = HID_KEY_ENTER; break;
                case '\t': *keycode = HID_KEY_TAB; break;
                
                // Shiftなし
                case '-':  *keycode = HID_KEY_MINUS; break;
                case '^':  *keycode = HID_KEY_EQUAL; break;         // JISの^
                case '@':  *keycode = HID_KEY_BRACKET_LEFT; break;  // JISの@
                case '[':  *keycode = HID_KEY_BRACKET_RIGHT; break; // JISの[
                case ';':  *keycode = HID_KEY_SEMICOLON; break;
                case ':':  *keycode = HID_KEY_APOSTROPHE; break;    // JISの:
                case ']':  *keycode = HID_KEY_BACKSLASH; break;     // JISの]
                case ',':  *keycode = HID_KEY_COMMA; break;
                case '.':  *keycode = HID_KEY_PERIOD; break;
                case '/':  *keycode = HID_KEY_SLASH; break;
                // case '\\': *keycode = HID_KEY_INTERNATIONAL3; break; // JISの￥(変換が必要な場合あり)
                case '\\': *keycode = HID_KEY_BACKSLASH; break; // JISの￥(変換が必要な場合あり)

                // Shiftあり
                case '!':  *keycode = HID_KEY_1; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '\"': *keycode = HID_KEY_2; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '#':  *keycode = HID_KEY_3; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '$':  *keycode = HID_KEY_4; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '%':  *keycode = HID_KEY_5; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '&':  *keycode = HID_KEY_6; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '\'': *keycode = HID_KEY_7; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '(':  *keycode = HID_KEY_8; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case ')':  *keycode = HID_KEY_9; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '=':  *keycode = HID_KEY_MINUS; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '~':  *keycode = HID_KEY_EQUAL; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '`':  *keycode = HID_KEY_BRACKET_LEFT; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '{':  *keycode = HID_KEY_BRACKET_RIGHT; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '+':  *keycode = HID_KEY_SEMICOLON; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '*':  *keycode = HID_KEY_APOSTROPHE; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '}':  *keycode = HID_KEY_BACKSLASH; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '<':  *keycode = HID_KEY_COMMA; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '>':  *keycode = HID_KEY_PERIOD; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                case '?':  *keycode = HID_KEY_SLASH; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
                // case '_':  *keycode = HID_KEY_UNDERSCORE; *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break; // JISのアンダーバー

                default:   *keycode = 0; break;
            }
        }
     }
}


void usb_hid_print_string(const char *str) {
     uint8_t key_none[6] = {0};

     for (int i = 0; str[i] != '\0'; i++) {
         uint8_t keycode = 0;
         uint8_t modifier = 0;

         ascii_to_hid_with_modifier(str[i], &keycode, &modifier);

         if (keycode != 0) {
             uint8_t key_report[6] = {keycode, 0, 0, 0, 0, 0};

             // キーを押す
             tinyusb_hid_keyboard_report(key_report, modifier);
             vTaskDelay(pdMS_TO_TICKS(15));

             // キーを離す
             tinyusb_hid_keyboard_report(key_none, 0);
             vTaskDelay(pdMS_TO_TICKS(15));
         }
     }
}