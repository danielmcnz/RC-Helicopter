#include "heliState.h"

static heliState heli_state;

// static bool prevSwitch;

void initHeliState(void)
{
    heli_state = Landed;
}

heliState getHeliState(void)
{
    return heli_state;
}

char* getHeliStateStr(void)
{
    switch(heli_state)
    {
    case Landed:
        return "landed\0";
    case Taking_Off:
        return "taking off\0";
    case Flying:
        return "flying\0";
    case Landing:
        return "landing\0";
    default:
        return "unknown\0";
    }
}

void setHeliSate(heliState state)
{
    heli_state = state;
}
