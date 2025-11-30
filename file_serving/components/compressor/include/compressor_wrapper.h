#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the compressor model (if needed)
 */
void compressor_init(void);

/**
 * @brief Run inference using the compressor model.
 * 
 * @param features Input features array (size 16)
 * @param prediction Output prediction value (pointer to float)
 * @return 0 on success, non-zero on error
 */
int compressor_predict(float *features, float *prediction);

#ifdef __cplusplus
}
#endif
