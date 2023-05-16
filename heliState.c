//**********************************************************
// File: heliState.c

// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Contains the definitions of the different states during
// flight, and allows for changing and reading of the
// state of the heli throught the program
//
//**********************************************************

#include "heliState.h"

//**********************************************************
// Static Variable
//**********************************************************

// HeliState can be updated from any file using the provided functions
// Current State
static heliState heli_state;

//**********************************************************
// initHeliState: Sets the inital state of the heli to
// "LANDED"
//**********************************************************
void initHeliState(void)
{
    heli_state = LANDED;
}

//**********************************************************
// getHeliState: Returns the current state of the heli
//**********************************************************
heliState getHeliState(void)
{
    return heli_state;
}

//**********************************************************
// getHeliStateStr: Returns the current state of the heli as a char
// pointer (str)
//**********************************************************
char* getHeliStateStr(void)
{
    switch(heli_state)
    {
    case LANDED:
        return "landed\0";
    case TAKING_OFF:
        return "taking off\0";
    case FLYING:
        return "flying\0";
    case LANDING:
        return "landing\0";
    default:
        return "unknown\0";
    }
}

//**********************************************************
// setHeliState: Sets the state of the heli to the given
// state
//**********************************************************
void setHeliState(heliState state)
{
    heli_state = state;
}
