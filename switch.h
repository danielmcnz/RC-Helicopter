//**********************************************************
// File: switch.h

// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Support for switch input, for the Tiva board, handles
// up and down pushes
//
//**********************************************************

#ifndef SWITCH_H_
#define SWITCH_H_

#include <stdint.h>
#include <stdbool.h>

// Enum for the differnt states of a switch imput 
typedef enum {
    SWITCH_NO_CHANGE = 0,
    SWITCH_UP,
    SWITCH_DOWN
} switch_state_t;

// Struct for switch, contains current switch position,
// new position, and whether the switch has changed
// since last checked
typedef struct
{
    bool switch_position;
    bool check_switch_position;
    bool switch_changed;
} switch_t;

//**********************************************************
// initSwitch: Initialises Switch_1
//**********************************************************
void initSwitch(void);

//**********************************************************
// updateSwitch: Checks new switch position and sees
// if switch_1's positions has changed
//**********************************************************
void updateSwitch(void);

//**********************************************************
// checkSwitch: Checks switch_1 and returns if it has not
// changed since last check, and then its positon
//**********************************************************
switch_state_t checkSwitch(void);

#endif // SWITCH_H_
