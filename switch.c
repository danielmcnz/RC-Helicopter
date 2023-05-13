#include "switch.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"

#define PERIPH_SWITCH_1     SYSCTL_PERIPH_GPIOA
#define PORT_BASE_SWITCH_1  GPIO_PORTA_BASE
#define PIN_SWITCH_1        GPIO_PIN_7

static switch_t switch_1;

void initSwitch(void)
{
    SysCtlPeripheralEnable(PERIPH_SWITCH_1);
    GPIOPinTypeGPIOInput(PORT_BASE_SWITCH_1, PIN_SWITCH_1);
    GPIOPadConfigSet(PORT_BASE_SWITCH_1, PIN_SWITCH_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    switch_1.switch_position = GPIOPinRead(PORT_BASE_SWITCH_1, PIN_SWITCH_1) == PIN_SWITCH_1;
}

void checkSwitch(void)
{
    switch_1.check_switch_position = GPIOPinRead(PORT_BASE_SWITCH_1, PIN_SWITCH_1) == PIN_SWITCH_1;

    if (switch_1.check_switch_position != switch_1.switch_position)
    {
        switch_1.switch_changed = true;
    }
    switch_1.switch_position = switch_1.check_switch_position;
}

switch_state_t getSwitch(void)
{
    if (switch_1.switch_changed)
    {
        switch_1.switch_changed = false;

        if (switch_1.switch_position)
        {
            return Switch_Active;
        }
        return Switch_Deactive;
    }
    return Switch_Same_State;
}
