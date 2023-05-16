//**********************************************************
// File: switch.c

// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Support for switch input, for the Tiva board, handles
// up and down pushes
//
//**********************************************************

#include "switch.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"

//**********************************************************
// Constants
//**********************************************************

#define PERIPH_SWITCH_1     SYSCTL_PERIPH_GPIOA
#define PORT_BASE_SWITCH_1  GPIO_PORTA_BASE
#define PIN_SWITCH_1        GPIO_PIN_7

//**********************************************************
// Static Variables
//**********************************************************

// Struct of current switch_1 position, new position, and
// whether the switch has changed since last checked
static switch_t switch_1;

//**********************************************************
// initSwitch: Initialises Switch_1
//**********************************************************
void initSwitch(void)
{
    SysCtlPeripheralEnable(PERIPH_SWITCH_1);
    GPIOPinTypeGPIOInput(PORT_BASE_SWITCH_1, PIN_SWITCH_1);
    GPIOPadConfigSet(PORT_BASE_SWITCH_1, PIN_SWITCH_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    switch_1.switch_position = GPIOPinRead(PORT_BASE_SWITCH_1, PIN_SWITCH_1) == PIN_SWITCH_1;
    switch_1.switch_changed = false;
}

//**********************************************************
// updateSwitch: Checks new switch position and sees
// if switch_1's positions has changed
//**********************************************************
void updateSwitch(void)
{
    switch_1.check_switch_position = GPIOPinRead(PORT_BASE_SWITCH_1, PIN_SWITCH_1) == PIN_SWITCH_1;

    if (switch_1.check_switch_position != switch_1.switch_position)
    {
        switch_1.switch_changed = true;
    }
    switch_1.switch_position = switch_1.check_switch_position;
}

//**********************************************************
// checkSwitch: Checks switch_1 and returns if it has not
// changed since last check, and then its positon
//**********************************************************
switch_state_t checkSwitch(void)
{
    if (switch_1.switch_changed)
    {
        switch_1.switch_changed = false;

        if (switch_1.switch_position)
        {
            return SWITCH_UP;
        }
        return SWITCH_DOWN;
    }
    return SWITCH_NO_CHANGE;
}
