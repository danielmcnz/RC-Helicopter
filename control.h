#include <stdint.h>
#include <stdbool.h>

#define ALTITUDE_KP 60
#define ALTITUDE_KD 10
#define ALTITUDE_KI 200

#define YAW_KP 5
#define YAW_KD 3
#define YAW_KI 2

void initControl(uint8_t update_freq);

void updateControl(void);
