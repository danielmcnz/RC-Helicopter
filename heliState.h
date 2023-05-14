#ifndef HELI_STATE_H_
#define HELI_STATE_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    Landed = 0,
    Taking_Off,
    Flying,
    Landing
} heliState;

void initHeliState(void);

heliState getHeliState(void);

void setHeliSate(heliState state);

char* getHeliStateStr(void);

#endif
