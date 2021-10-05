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

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT(
             MO(1),
    KC_1,    KC_2,    KC_3,
    KC_4,    KC_5,    KC_6
  ),
  [1] = LAYOUT(
             _______,
    RGB_MOD, KC_UP,   RESET,
    KC_LEFT, KC_DOWN, KC_RGHT
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

void rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    int layer = get_highest_layer(layer_state|default_layer_state);

    for (uint8_t i = led_min; i <= led_max; i++) {
        HSV hsv = rgb_matrix_get_hsv();
        hsv.h += layer * 64;
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
}