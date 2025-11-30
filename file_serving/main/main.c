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

#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "file_serving_example_common.h"
#include "compressor_wrapper.h"
#include "turbine_wrapper.h"
#include "ssd1306.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"

/* This example demonstrates how to create file server
 * using esp_http_server. This file has only startup code.
 * Look in file_server.c for the implementation.
 */

static const char *TAG = "example";

#define ESP_WIFI_SSID      "ESP32-Anomaly"
#define ESP_WIFI_PASS      "12345678"
#define ESP_WIFI_CHANNEL   1
#define MAX_STA_CONN       4

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

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                .required = false,
            },
        },
    };
    if (strlen(ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             ESP_WIFI_SSID, ESP_WIFI_PASS, ESP_WIFI_CHANNEL);
}

void display_task(void *pvParameter)
{
    char ip_str[32] = "Wait IP...";
    bool ip_acquired = false;

    while (1) {
        if (!ip_acquired) {
             esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
             if (netif) {
                 esp_netif_ip_info_t ip_info;
                 if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
                     if (ip_info.ip.addr != 0) {
                         sprintf(ip_str, IPSTR, IP2STR(&ip_info.ip));
                         ip_acquired = true;
                     }
                 }
             }
        }
        
        float c_val = get_last_compressor_val();
        float t_val = get_last_turbine_val();
        float c_thresh = get_compressor_threshold();
        float t_thresh = get_turbine_threshold();

        ssd1306_display_status(c_val, c_thresh, t_val, t_thresh, ip_str, ESP_WIFI_SSID, ESP_WIFI_PASS);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
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
    
    /* Initialize Display */
    ssd1306_init();
    xTaskCreate(display_task, "display_task", 4096, NULL, 5, NULL);

    /* Run inference test */
    run_inference_test();

    /* Initialize Wi-Fi as SoftAP */
    wifi_init_softap();

    /* Start the file server */
    ESP_ERROR_CHECK(example_start_file_server(base_path));
    ESP_LOGI(TAG, "File server started");
}
