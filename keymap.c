#include "helix.h"
#include "bootloader.h"
#include "action_layer.h"
#include "eeconfig.h"
#ifdef PROTOCOL_LUFA
#include "lufa.h"
#include "split_util.h"
#endif
#include "LUFA/Drivers/Peripheral/TWI.h"
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif

extern keymap_config_t keymap_config;

#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

extern uint8_t is_master;

#define _QWERTY 0
#define _LOWER 1
#define _RAISE 2
#define _FUNCT 3
#define _ARROW 4
#define _ADJUST 5
#define _ADMINI 6

#define _______ KC_TRNS
#define XXXXXXX KC_NO

#define NEXTWKS ACTION_MODS_KEY(MOD_LCTL, KC_RGHT)
#define PREVWKS ACTION_MODS_KEY(MOD_LCTL, KC_LEFT)
#define NEXTTAB ACTION_MODS_KEY(MOD_LGUI, KC_RCBR)
#define PREVTAB ACTION_MODS_KEY(MOD_LGUI, KC_LCBR)
#define WINCOPY ACTION_MODS_KEY(MOD_LCTL, KC_C)
#define WINPASTE ACTION_MODS_KEY(MOD_LCTL, KC_V)
#define MACUNDO ACTION_MODS_KEY(MOD_LGUI, KC_Z)
#define MACCUT ACTION_MODS_KEY(MOD_LGUI, KC_X)
#define MACCOPY ACTION_MODS_KEY(MOD_LGUI, KC_C)
#define MACPASTE ACTION_MODS_KEY(MOD_LGUI, KC_V)
#define MACFIND ACTION_MODS_KEY(MOD_LGUI, KC_F)
#define SPTLGHT ACTION_MODS_KEY(MOD_LGUI, KC_SPC)
#define MISSIONCTL ACTION_MODS_KEY(MOD_LCTL, KC_UP)
#define NEXTAPP ACTION_MODS_KEY(MOD_LGUI, KC_TAB)
#define PREVAPP ACTION_MODS_KEY(MOD_LGUI | MOD_LSFT, KC_TAB)

//Macros
#define SEND_KEYMAP_URI 0
#define MAC_COPY_PASTE 1
#define WIN_COPY_PASTE 2

// TAP DANCE
enum {
    TD_CTL_SPL = 0,
    TD_PRN_PREVWK,
    TD_PRN_NEXTWK
};


enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  LOWER,
  RAISE,
  FUNCT,
  ARROW,
  ADJUST,
  ADMINI,
  RGBRST
  // DYNAMIC_MACRO_RANGE
};

// #include "dynamic_macro.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  /* Qwerty
   * ,-----------------------------------------.              ,-----------------------------------------.
   * | Tab  |   Q  |   W  |   E  |   R  |   T  |              |   Y  |   U  |   I  |   O  |   P  |  \   |
   * |------+------+------+------+------+------|              |------+------+------+------+------+------|
   * |TD_SPL|      |      |      |      |      |              |      |      |      |      | Arrow|  '   |
   * | Ctrl |   A  |   S  |   D  |   F  |   G  |              |   H  |   J  |   K  |   L  |   ;  | Ctrl |
   * |------+------+------+------+------+------|              |------+------+------+------+------+------|
   * |  [   |   Z  |      |      |      |      |              |      |      |      |      |   /  |   ]  |
   * | Shift| Raise|   X  |   C  |   V  |   B  |              |   N  |   M  |   ,  |   .  | Lower| Shift|
   * |------+------+------+------+------+------|--------------|------+------+------+------+------+------|
   * |      | Left |      |      |      | BS   |      |       | Esc  |      |TD_PWK|TD_NWK| Right|      |
   * |Adjust| Func | Alt  | GUI  |Space | Lower|      |       | Raise| Enter|   (  |   )  | Func | Arrow|
   * `--------------------------------------------------------------------------------------------------'
   */
  [_QWERTY] = KEYMAP( \
    KC_TAB,  KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSLS, \
    TD(TD_CTL_SPL), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, LT(_ARROW, KC_SCLN), CTL_T(KC_QUOT), \
    SFT_T(KC_LBRC), LT(_RAISE, KC_Z), KC_X, KC_C, KC_V, KC_B, \
    KC_N, KC_M, KC_COMM, KC_DOT, LT(_LOWER, KC_SLSH), SFT_T(KC_RBRC), \
    TT(_ADJUST), LT(_FUNCT, KC_LEFT), KC_LALT, KC_LGUI, KC_SPC, LT(_LOWER, KC_BSPC), \
    _______, _______, \
    LT(_RAISE, KC_ESC), KC_ENT, TD(TD_PRN_PREVWK), TD(TD_PRN_NEXTWK), LT(_FUNCT, KC_RGHT), TT(_ARROW) \
  ),

  /* Lower
   * ,-----------------------------------------.             ,-----------------------------------------.
   * |   ~  |   !  |   @  |   #  |   $  |   %  |             |   ^  |   &  |   *  |   (  |   )  | Bksp |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |      |      |      |M FIND|      |             |      |   _  |   +  |   {  |   }  | Del  |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |M UNDO|M CUT |M COPY|MPASTE|      |             |      |      |      |      |XXXXXX|      |
   * |------+------+------+------+------+------|-------------|------+------+------+------+------+------|
   * |      |      |      |      |      |XXXXXX|      |      |      |      |      |      |      |      |
   * `-------------------------------------------------------------------------------------------------'
   */
  [_LOWER] = KEYMAP( \
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC, \
    _______, _______, _______, _______, MACFIND, _______, _______, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_DEL, \
    _______, MACUNDO, MACCUT,  MACCOPY, MACPASTE, _______, _______, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, _______, _______, \
    _______, _______, \
    _______, _______, _______, _______, _______, _______ \
  ),

  /* Raise
   * ,-----------------------------------------.             ,-----------------------------------------.
   * |   `  |   1  |   2  |   3  |   4  |   5  |             |   6  |   7  |   8  |   9  |   0  | Bksp |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |      |      |      |M FIND|      |             |      |   -  |   =  |   [  |   ]  | Del  |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |XXXXXX|M CUT |M COPY|MPASTE|      |             |      |      |      |      |      |      |
   * |------+------+------+------+------+------|-------------|------+------+------+------+------+------|
   * |      |      |      |      |      |      |      |      |XXXXXX|      |      |      |      |      |
   * `-------------------------------------------------------------------------------------------------'
   */
  [_RAISE] = KEYMAP( \
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC, \
    _______, _______, _______, _______, MACFIND, _______, _______, KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_DEL, \
    _______, _______, MACCUT,  MACCOPY, MACPASTE, _______, _______, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, _______, _______, \
    _______, _______, \
    _______, _______, _______, _______, _______, _______ \
  ),

  /* Function
   * ,-----------------------------------------.             ,-----------------------------------------.
   * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |             |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |      |M C/P |M COPY|MPASTE|SPTLGT|             |MSNCTL|      |      |      |      |      |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |      |W C/P |W COPY|WPASTE|PREVWK|             |NEXTWK|      |      |KEYMAP|      |      |
   * |------+------+------+------+------+------|-------------|------+------+------+------+------+------|
   * |      |XXXXXX|      |      |      |      |      |      |      |      |      |      |XXXXXX|      |
   * `-------------------------------------------------------------------------------------------------'
   */
  [_FUNCT] = KEYMAP( \
    KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, \
    _______, _______, M(MAC_COPY_PASTE), MACCOPY, MACPASTE, SPTLGHT, \
    MISSIONCTL, _______, _______, _______, _______, _______, \
    _______, _______, M(WIN_COPY_PASTE), WINCOPY, WINPASTE, PREVWKS, \
    NEXTWKS, _______, _______, M(SEND_KEYMAP_URI), _______, _______, \
    _______, _______, _______, _______, _______, _______, \
    _______, _______, \
    _______, _______, _______, _______, _______, _______ \
  ),

  /* Arrow
   * ,-----------------------------------------.             ,-----------------------------------------.
   * |      |      |      |  Up  |      |      |             |      |      |      |      |      |      |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |      | Left | Down | Right|      |             | Left | Down |  Up  |Right |XXXXXX|      |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |      |      |      |      |      |             |      |      |      |      |      |      |
   * |------+------+------+------+------+------|-------------|------+------+------+------+------+------|
   * |ADMINI|      |      |      |      |      |      |      |      |      |      |      |      |XXXXXX|
   * `-------------------------------------------------------------------------------------------------'
   */
  [_ARROW] =  KEYMAP( \
    _______, _______, _______, KC_UP, _______, _______, _______, _______, _______, _______, _______, _______, \
    _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, _______, _______, \
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
    ADMINI, _______, _______, _______, _______, _______, \
    _______, _______, \
    _______, _______, _______, _______, _______, _______ \
  ),

  /* Adjust
   * ,-----------------------------------------.             ,-----------------------------------------.
   * |      |      |      |      |      |PREVTB|             |NEXTTB|      |      |      |      |      |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |      |      |      |      |SPTLGT|             |MSNCTL|      |      |      |      |      |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |      |      |      |      |PREVWK|             |NEXTWK|      |      |      |      |      |
   * |------+------+------+------+------+------|-------------|------+------+------+------+------+------|
   * |XXXXXX|      |      |      |      |      |      |      |      |      |      |      |      |ADMINI|
   * `-------------------------------------------------------------------------------------------------'
   */
  [_ADJUST] =  KEYMAP( \
    _______, _______, _______, _______, _______, PREVTAB, NEXTTAB, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, _______, SPTLGHT, MISSIONCTL, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, _______, PREVWKS, NEXTWKS, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, _______, _______, \
    _______, _______, \
    _______, _______, _______, _______, _______, ADMINI \
  ),

  /* administrator
   * ,-----------------------------------------.             ,-----------------------------------------.
   * |RESET |      |      |      |      |      |             |      |      |      |      |      |      |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |RGB on|RGBmod|      |      |      |      |             |      |      |      |      |RGB v-|RGB v+|
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |RGB h-|RGB h+|      |      |      |      |             |      |      |      |      |RGB s-|RGB s+|
   * |------+------+------+------+------+------|-------------|------+------+------+------+------+------|
   * |XXXXXX|      |      |      |      |      |      |      |      |      |      |      |      |XXXXXX|
   * `-------------------------------------------------------------------------------------------------'
   */
  [_ADMINI] =  KEYMAP( \
    RESET, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
    RGB_TOG, RGB_MOD, _______, _______, _______, _______, _______, _______, _______, _______, RGB_VAD, RGB_VAI, \
    RGB_HUD, RGB_HUI, _______, _______, _______, _______, _______, _______, _______, _______, RGB_SAD, RGB_SAI, \
    _______, _______, _______, _______, _______, _______, \
    _______, _______, \
    _______, _______, _______, _______, _______, _______ \
  )

};

static uint16_t start;
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    switch(id) {
        case SEND_KEYMAP_URI:
            if (record->event.pressed) {
                SEND_STRING("https://github.com/rinx/qmk_firmware_helix_rinx/blob/master/keymap.c");
            }
            break;
        case MAC_COPY_PASTE:
            if (record->event.pressed) {
                start = timer_read();
            } else {
                if (timer_elapsed(start) >= TAPPING_TERM)
                    return MACRO(D(LGUI), T(C), U(LGUI), END);
                else
                    return MACRO(D(LGUI), T(V), U(LGUI), END);
            }
            break;
        case WIN_COPY_PASTE:
            if (record->event.pressed) {
                start = timer_read();
            } else {
                if (timer_elapsed(start) >= TAPPING_TERM)
                    return MACRO(D(LCTL), T(C), U(LCTL), END);
                else
                    return MACRO(D(LCTL), T(V), U(LCTL), END);
            }
            break;
    }
    return MACRO_NONE;
};

//Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
  [TD_CTL_SPL] = ACTION_TAP_DANCE_DOUBLE(KC_LCTL, SPTLGHT),
  [TD_PRN_PREVWK] = ACTION_TAP_DANCE_DOUBLE(KC_LPRN, PREVWKS),
  [TD_PRN_NEXTWK] = ACTION_TAP_DANCE_DOUBLE(KC_RPRN, NEXTWKS)
};

// define variables for reactive RGB
bool TOG_STATUS = false;
int RGB_current_mode;

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  // if (!process_record_dynamic_macro(keycode, record)) {
  //   return false;
  // }

  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        persistent_default_layer_set(1UL<<_QWERTY);
      }
      return false;
      break;
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
      } else {
        layer_off(_LOWER);
      }
      return false;
      break;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
      } else {
        layer_off(_RAISE);
      }
      return false;
      break;
    case FUNCT:
      if (record->event.pressed) {
        layer_on(_FUNCT);
      } else {
        layer_off(_FUNCT);
      }
      return false;
      break;
    case ARROW:
      if (record->event.pressed) {
        layer_on(_ARROW);
      } else {
        layer_off(_ARROW);
      }
      return false;
      break;
    case ADJUST:
      if (record->event.pressed) {
        layer_on(_ADJUST);
      } else {
        layer_off(_ADJUST);
      }
      return false;
      break;
    case ADMINI:
      if (record->event.pressed) {
        layer_on(_ADMINI);
      } else {
        layer_off(_ADMINI);
      }
      return false;
      break;
    //led operations - RGB mode change now updates the RGB_current_mode to allow the right RGB mode to be set after reactive keys are released
    case RGB_MOD:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          rgblight_mode(RGB_current_mode);
          rgblight_step();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      return false;
      break;
    case RGBRST:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgblight_default();
          rgblight_enable();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      break;
  }
  return true;
}

void matrix_init_user(void) {
    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
    //SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
    #ifdef SSD1306OLED
        TWI_Init(TWI_BIT_PRESCALE_1, TWI_BITLENGTH_FROM_FREQ(1, 800000));
        iota_gfx_init(!has_usb());   // turns on the display
    #endif
}

//SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED

void matrix_scan_user(void) {
     iota_gfx_task();  // this is what updates the display continuously
}

void matrix_update(struct CharacterMatrix *dest,
                          const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

void render_status(struct CharacterMatrix *matrix) {

  static char logo[]={
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,
    0};
  matrix_write(matrix, logo);

  // Define layers here, Have not worked out how to have text displayed for each layer. Copy down the number you see and add a case for it below
  char buf[40];
  snprintf(buf,sizeof(buf), "Undef-%ld", layer_state);

  uint8_t layer = biton32(layer_state);
  matrix_write_P(matrix, PSTR("Layer: "));
    switch (layer) {
        case _QWERTY:
            matrix_write_P(matrix, PSTR("QWERTY"));
            break;
        case _LOWER:
            matrix_write_P(matrix, PSTR("Lower"));
            break;
        case _RAISE:
            matrix_write_P(matrix, PSTR("Raise"));
            break;
        case _FUNCT:
            matrix_write_P(matrix, PSTR("Function"));
            break;
        case _ARROW:
            matrix_write_P(matrix, PSTR("Arrow"));
            break;
        case _ADJUST:
            matrix_write_P(matrix, PSTR("Adjust"));
            break;
        case _ADMINI:
            matrix_write_P(matrix, PSTR("Administrator"));
            break;
        default:
            matrix_write(matrix, buf);
            break;
    }
}

void iota_gfx_task_user(void) {
  struct CharacterMatrix matrix;

#if DEBUG_TO_SCREEN
  if (debug_enable) {
    return;
  }
#endif

  matrix_clear(&matrix);
  if(is_master){
    render_status(&matrix);
  }
  matrix_update(&display, &matrix);
}

#endif
