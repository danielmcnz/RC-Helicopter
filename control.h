#include <stdint.h>
#include <stdbool.h>
#include "rotors.h"
#include "altitude.h"
#include "yaw.h"

#define ALTITUDE_KP 60
#define ALTITUDE_KD 10
#define ALTITUDE_KI 400

#define YAW_KP 5
#define YAW_KD 3
#define YAW_KI 2

void initControl(uint8_t update_freq);

void updateControl(void);
