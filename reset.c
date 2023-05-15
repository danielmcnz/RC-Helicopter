/*
 * reset.c
 *
 *  Created on: 15/05/2023
 *      Author: dmc270
 */

#include "reset.h"

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

void resetIntHandler(void)
{
    SysCtlReset();
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_6);
}

void initReset()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_6);

    GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_FALLING_EDGE);

    GPIOIntRegister(GPIO_PORTA_BASE, resetIntHandler);
    GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_6);
}
