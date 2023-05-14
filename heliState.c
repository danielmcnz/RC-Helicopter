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
        return "landed";
    case Taking_Off:
        return "taking off";
    case Flying:
        return "flying";
    case Landing:
        return "landing";
    }
}

void setHeliSate(heliState state)
{
    heli_state = state;
}
