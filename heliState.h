//**********************************************************
// File: heliState.h

// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Contains the definitions of the different states during
// flight, and allows for changing and reading of the
// state of the heli throught the program
//
//**********************************************************

#ifndef HELI_STATE_H_
#define HELI_STATE_H_

#include <stdint.h>
#include <stdbool.h>

// Enum containing the four desired states during flight
typedef enum {
    LANDED = 0,
    TAKING_OFF,
    FLYING,
    LANDING
} heliState;

//**********************************************************
// initHeliState: Sets the inital state of the heli to
// "LANDED"
//**********************************************************
void initHeliState(void);

//**********************************************************
// getHeliState: Returns the current state of the heli
//**********************************************************
heliState getHeliState(void);

//**********************************************************
// setHeliState: Sets the state of the heli to the given
// state
//**********************************************************
void setHeliState(heliState state);

//**********************************************************
// getHeliStateStr: Returns the current state of the heli as a char
// pointer (str)
//**********************************************************
char* getHeliStateStr(void);

#endif // HELI_STATE_H_
