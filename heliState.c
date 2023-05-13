#include "heliState.h"

static heliState heli_state;

static bool prevSwitch;

void initHeliState(void)
{
    heli_state = Landed;
}

heliState getHeliState()
{
    return heli_state;
}

void setHeliSate(heliState state)
{
    heli_state = state;
}