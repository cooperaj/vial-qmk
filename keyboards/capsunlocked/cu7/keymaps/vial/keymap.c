/* Copyright 2021 CapsUnlocked
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include <lib/lib8tion/lib8tion.h>

#ifdef RGB_MATRIX_CUSTOM_USER
enum my_keycodes {
    RGB_LYR = USER00,
};

typedef union {
    uint8_t raw;
    struct {
        bool rgb_solid_reactive_layer :1;
    };
} user_config_t;

user_config_t user_config;
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT(
             MO(1),
    KC_1,    KC_2,    KC_3,
    KC_4,    KC_5,    KC_6
  ),
  [1] = LAYOUT(
             _______,
    RGB_MOD, RGB_LYR,   RESET,
    _______, _______, _______
  ),
  [2] = LAYOUT(
             _______,
    _______, _______, _______,
    _______, _______, _______
  ),
  [3] = LAYOUT(
             _______,
    _______, _______, _______,
    _______, _______, _______
  ),
};

#ifdef RGB_MATRIX_CUSTOM_USER
void keyboard_post_init_user(void) {
    user_config.raw = eeconfig_read_user();

    if (user_config.rgb_solid_reactive_layer) {
        rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_SOLID_REACTIVE_LAYER);
    }
}

void eeconfig_init_user(void) { // EEPROM is getting reset!
    user_config.raw = 0;
    user_config.rgb_solid_reactive_layer = false; // off by default
    eeconfig_update_user(user_config.raw);

    rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_SOLID_REACTIVE_LAYER);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case RGB_LYR:  // toggle layer colour changes using reactive rgb_matrix
            if (record->event.pressed) {
                user_config.rgb_solid_reactive_layer ^= 1; // Toggles the status
                eeconfig_update_user(user_config.raw);
                if (user_config.rgb_solid_reactive_layer) {
                    rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_SOLID_REACTIVE_LAYER);
                }
            }
            return false;
        case RGB_MODE_FORWARD ... RGB_MODE_GRADIENT: // For any of the RGB codes (see quantum_keycodes.h, L400 for reference)
            if (record->event.pressed) { //This disables layer indication, as it's assumed that if you're changing this ... you want that disabled
                if (user_config.rgb_solid_reactive_layer) {        // only if this is enabled
                    user_config.rgb_solid_reactive_layer = false;  // disable it, and
                    eeconfig_update_user(user_config.raw); // write the setings to EEPROM
                }
            }
            return true;
        default:
            return true; // Process all other keycodes normally
    }
}
#endif

#ifdef SIMPLE_CU7_LAYER_RGB
// Default layer gets to use fancy matrix animations but any other layers
// get a static colour calculated from the default.
void rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    int layer = get_highest_layer(layer_state|default_layer_state);
  
    if (layer > 0) {
        HSV user_set_hsv = rgb_matrix_get_hsv();
        HSV hsv = {addmod8(user_set_hsv.h, layer * 48, 255), user_set_hsv.s, user_set_hsv.v};
        RGB rgb = hsv_to_rgb(hsv);

        for (uint8_t i = led_min; i <= led_max; i++) {
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }
}
#endif