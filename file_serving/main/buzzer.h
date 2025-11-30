#ifndef BUZZER_H
#define BUZZER_H

#include "sdkconfig.h"

#if defined(CONFIG_IDF_TARGET_ESP32C3)
#define BUZZER_GPIO 5
#else
#define BUZZER_GPIO 27
#endif

void buzzer_init(void);
void buzzer_trigger(void);

#endif
