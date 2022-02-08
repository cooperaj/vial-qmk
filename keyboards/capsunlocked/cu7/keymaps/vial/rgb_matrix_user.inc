#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED

RGB_MATRIX_EFFECT(SOLID_REACTIVE_LAYER)
RGB_MATRIX_EFFECT(SOLID_REACTIVE_LAYER_FLASH)

    #ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS


static HSV SOLID_REACTIVE_LAYER_math(HSV hsv, uint16_t offset) {
    int layer = get_highest_layer(layer_state|default_layer_state);

    if (layer > 0) {
        hsv.h = addmod8(hsv.h, layer * 48, 255);
    }

    hsv.h += qsub8(130, offset);
    return hsv;
}

static bool SOLID_REACTIVE_LAYER(effect_params_t* params) { return effect_runner_reactive(params, &SOLID_REACTIVE_LAYER_math); }

typedef struct PACKED {
    uint8_t layer;
    HSV layer_hue;
    uint16_t max_tick;
    uint16_t tick;
} layer_tick_t;

static layer_tick_t g_layer_tick;

static void SOLID_REACTIVE_LAYER_FLASH_init(void) {
    g_layer_tick.layer = 0;
    HSV hsv = {rgb_matrix_config.hsv.h, rgb_matrix_config.hsv.s, rgb_matrix_config.hsv.v};
    g_layer_tick.layer_hue = hsv;
    g_layer_tick.max_tick = UINT16_MAX / 255; // the max duration of the animation
    g_layer_tick.tick = g_layer_tick.max_tick;
}

static uint16_t SOLID_REACTIVE_LAYER_FLASH_track_tick(void) {
    // setup and track our own tick
    uint32_t deltaTime = sync_timer_elapsed32(g_rgb_timer);
    if (UINT16_MAX - deltaTime < g_layer_tick.tick) { 
        g_layer_tick.tick = UINT16_MAX;
    } else {
        g_layer_tick.tick += deltaTime;
    }

    // ensure our tick value is capped at the scaled one
    uint16_t tick = g_layer_tick.max_tick;
    if (g_layer_tick.tick < tick) {
       tick = g_layer_tick.tick;
    }

    return tick;
}

static bool SOLID_REACTIVE_LAYER_FLASH(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    if (params->init) SOLID_REACTIVE_LAYER_FLASH_init();

    uint16_t tick = SOLID_REACTIVE_LAYER_FLASH_track_tick();

    // configured animation base colour
    HSV hsv = {rgb_matrix_config.hsv.h, rgb_matrix_config.hsv.s, rgb_matrix_config.hsv.v};

    // has the layer changed?
    int layer = get_highest_layer(layer_state|default_layer_state);
    if (layer != g_layer_tick.layer) {
        g_layer_tick.layer = layer;
        g_layer_tick.layer_hue.h = addmod8(hsv.h, mul8(layer, 45), 255); // target layer hue
        g_layer_tick.tick = 0; // kick of the animation cycle
    }

    // calculate layer hue for current layer
    uint16_t anim_offset = scale16by8(tick, 255);
    if (layer > 0) {
        uint8_t cw = abs(add8(sub8(g_layer_tick.layer_hue.h, hsv.h), 255));
        uint8_t ccw = abs(add8(sub8(hsv.h, g_layer_tick.layer_hue.h), 255));
        uint8_t diff = cw < ccw ? cw : ccw;

        if (mod8(cw, 255) > 127) {
            hsv.h -= qsub8(diff, anim_offset);
        } else {
            hsv.h += qsub8(diff, anim_offset);
        }

        hsv.h = mod8(hsv.h, 255);
    }

    RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
    RGB layer_rgb = rgb_matrix_hsv_to_rgb(g_layer_tick.layer_hue);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        if (i < 3 && abs(i-3) == layer) {
            rgb_matrix_set_color(i, layer_rgb.r, layer_rgb.g, layer_rgb.b);
            continue;
        }

        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return led_max < DRIVER_LED_TOTAL;
}

    #endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif // RGB_MATRIX_KEYREACTIVE_ENABLED