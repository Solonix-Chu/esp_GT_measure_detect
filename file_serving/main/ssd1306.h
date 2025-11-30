#ifndef SSD1306_H
#define SSD1306_H

#include "esp_err.h"

// I2C Configuration
#define I2C_MASTER_SCL_IO           4      /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO           3      /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM              0       /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          100000  /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0       /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0       /*!< I2C master doesn't need buffer */

// SSD1306 Configuration
#define SSD1306_ADDR                0x3C
#define SSD1306_WIDTH               128
#define SSD1306_HEIGHT              64

void ssd1306_init(void);
void ssd1306_clear_screen(void);
void ssd1306_display_text(const void *arg_text);
void ssd1306_display_text_x_y(void *arg_text, uint8_t x, uint8_t y);
void ssd1306_display_status(float comp_val, float comp_thresh, float turb_val, float turb_thresh, const char* ip, const char* ssid, const char* pwd);

#endif
