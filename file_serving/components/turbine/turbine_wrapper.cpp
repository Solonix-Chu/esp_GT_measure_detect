#include "turbine_wrapper.h"
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"

void turbine_init(void) {
    // No specific initialization required
}

int turbine_predict(float *features, float *prediction) {
    ei_impulse_result_t result = { 0 };
    signal_t signal;
    
    int err = numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    if (err != 0) {
        ei_printf("ERR: signal_from_buffer failed (%d)\n", err);
        return err;
    }

    EI_IMPULSE_ERROR res = process_impulse(&ei_default_impulse, &signal, &result, false);
    if (res != EI_IMPULSE_OK) {
        ei_printf("ERR: run_classifier failed (%d)\n", res);
        return res;
    }

    if (prediction) {
        if (EI_CLASSIFIER_LABEL_COUNT > 0) {
             *prediction = result.classification[0].value;
        } else {
             *prediction = 0.0f;
        }
    }

    return 0;
}
