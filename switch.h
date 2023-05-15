#ifndef SWITCH_H_
#define SWITCH_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SWITCH_NO_CHANGE = 0,
    SWITCH_UP,
    SWITCH_DOWN
} switch_state_t;

typedef struct
{
    bool switch_position;
    bool check_switch_position;
    bool switch_changed = false;
} switch_t;


void initSwitch(void);
void updateSwitch(void);
switch_state_t checkSwitch(void);

#endif
