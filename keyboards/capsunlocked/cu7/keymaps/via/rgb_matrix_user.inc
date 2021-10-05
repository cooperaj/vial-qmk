#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
RGB_MATRIX_EFFECT(SOLID_REACTIVE_LAYER)

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

    #endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif // RGB_MATRIX_KEYREACTIVE_ENABLED