#ifndef HELI_STATE_H_
#define HELI_STATE_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LANDED = 0,
    TAKING_OFF,
    FLYING,
    LANDING
} heliState;

void initHeliState(void);

heliState getHeliState(void);

void setHeliState(heliState state);

char* getHeliStateStr(void);

#endif
