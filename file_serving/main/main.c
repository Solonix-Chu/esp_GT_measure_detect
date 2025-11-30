/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* HTTP File Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "file_serving_example_common.h"
#include "compressor_wrapper.h"
#include "turbine_wrapper.h"

/* This example demonstrates how to create file server
 * using esp_http_server. This file has only startup code.
 * Look in file_server.c for the implementation.
 */

static const char *TAG = "example";

typedef struct {
    float features[16];
    float expected_compressor_decay;
    float expected_turbine_decay;
    const char* description;
} test_case_t;

void run_inference_test() {
    test_case_t tests[] = {
        {
            .features = {1.138f, 3.0f, 289.964f, 1349.489f, 6677.38f, 7.584f, 7.584f, 464.006f, 
                         288.0f, 550.563f, 1.096f, 0.998f, 5.947f, 1.019f, 7.137f, 0.082f},
            .expected_compressor_decay = 0.95f,
            .expected_turbine_decay = 0.975f,
            .description = "Baseline (0.95, 0.975)"
        },
        {
            .features = {1.138f, 3.0f, 379.88f, 1355.375f, 6683.916f, 7.915f, 7.915f, 464.017f,
                         288.0f, 550.985f, 1.1f, 0.998f, 5.963f, 1.019f, 3.879f, 0.079f},
            .expected_compressor_decay = 0.95f,
            .expected_turbine_decay = 0.976f,
            .description = "Turbine variation (0.95, 0.976)"
        },
        {
            .features = {1.138f, 3.0f, 395.648f, 1331.563f, 6663.587f, 6.59f, 6.59f, 477.697f,
                         288.0f, 551.149f, 1.099f, 0.998f, 5.976f, 1.019f, 17.188f, 0.105f},
            .expected_compressor_decay = 0.951f,
            .expected_turbine_decay = 0.975f,
            .description = "Compressor variation (0.951, 0.975)"
        }
    };

    ESP_LOGI(TAG, "Running inference tests...");

    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        float compressor_decay = 0.0f;
        float turbine_decay = 0.0f;

        ESP_LOGI(TAG, "Test Case %d: %s", i + 1, tests[i].description);
        
        compressor_predict(tests[i].features, &compressor_decay);
        turbine_predict(tests[i].features, &turbine_decay);
        
        ESP_LOGI(TAG, "  Predicted Compressor Decay: %f (Expected: %.3f)", compressor_decay, tests[i].expected_compressor_decay);
        ESP_LOGI(TAG, "  Predicted Turbine Decay:    %f (Expected: %.3f)", turbine_decay, tests[i].expected_turbine_decay);
        
        // Anomaly check (Logic: Lower decay coefficient means worse health)
        // Note: Thresholds are arbitrary for this example, adjusted based on expected values
        if (compressor_decay < 0.95f) {
             ESP_LOGW(TAG, "  Compressor Anomaly Detected!");
        } else {
             ESP_LOGI(TAG, "  Compressor Status: OK");
    }
    
        if (turbine_decay < 0.975f) {
             ESP_LOGW(TAG, "  Turbine Anomaly Detected!");
        } else {
             ESP_LOGI(TAG, "  Turbine Status: OK");
        }
        ESP_LOGI(TAG, "--------------------------------------------------");
    }
}


void app_main(void)
{
    ESP_LOGI(TAG, "Starting example");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Initialize file storage */
    const char* base_path = "/data";
    ESP_ERROR_CHECK(example_mount_storage(base_path));

    /* Run inference test */
    run_inference_test();

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* Start the file server */
    ESP_ERROR_CHECK(example_start_file_server(base_path));
    ESP_LOGI(TAG, "File server started");
}
