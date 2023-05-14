#include "heliState.h"

static heliState heli_state;

// static bool prevSwitch;

void initHeliState(void)
{
    heli_state = LANDED;
}

heliState getHeliState(void)
{
    return heli_state;
}

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

void setHeliState(heliState state)
{
    heli_state = state;
}
