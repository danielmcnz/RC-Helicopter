#include <stdint.h>
#include <stdbool.h>
#include "rotors.h"
#include "altitude.h"
#include "yaw.h"

#define ALTITUDE_KP 0
#define ALTITUDE_KD 0
#define ALTITUDE_KI 0

#define YAW_KP 0
#define YAW_KD 0
#define YAW_KI 0



void initControl(void);
void updateControl(void);