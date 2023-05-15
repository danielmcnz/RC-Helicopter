#include <stdint.h>
#include <stdbool.h>

#define ALTITUDE_KP 650
#define ALTITUDE_KD 800
#define ALTITUDE_KI 12

#define YAW_KP 800
#define YAW_KD 800
#define YAW_KI 9

void initControl(uint8_t update_freq);

void updateControl(void);