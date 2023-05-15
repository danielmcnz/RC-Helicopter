#include <stdint.h>
#include <stdbool.h>

//#define ALTITUDE_KP 6
//#define ALTITUDE_KD 2
//#define ALTITUDE_KI 2
#define ALTITUDE_KP 650
#define ALTITUDE_KD 800
#define ALTITUDE_KI 12

#define YAW_KP 5
#define YAW_KD 3
#define YAW_KI 2

void initControl(uint8_t update_freq);

void updateControl(void);
