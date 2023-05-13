#ifndef SWITCH_H_
#define SWITCH_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    Switch_Same_State = 0,
    Switch_Active,
    Switch_Deactive
} switch_state_t;

typedef struct
{
    bool switch_position;
    bool check_switch_position;
    bool switch_changed;
} switch_t;


void initSwitch(void);
void checkSwitch(void);
switch_state_t getSwitch(void);

#endif
