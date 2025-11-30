#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the turbine model (if needed)
 */
void turbine_init(void);

/**
 * @brief Run inference using the turbine model.
 * 
 * @param features Input features array (size 16)
 * @param prediction Output prediction value (pointer to float)
 * @return 0 on success, non-zero on error
 */
int turbine_predict(float *features, float *prediction);

#ifdef __cplusplus
}
#endif
