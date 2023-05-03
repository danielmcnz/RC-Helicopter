/*
 * Yaw.cpp
 *
 *  Created on: 3/05/2023
 *      Author: dmc270
 */

#include "yaw.h"

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

void initYaw(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);

    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

void YawIntHandler(void)
{
    static int prevState = -1;

    int signalAB = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    switch(prevState)
    {
    case 0:
        signalAB == 1 ? yaw++ : yaw--;
        break;
    case 1:
        signalAB == 3 ? yaw++ : yaw--;
        break;
    case 2:
        signalAB == 0 ? yaw++ : yaw--;
        break;
    case 3:
        signalAB == 2 ? yaw++ : yaw--;
        break;
    case -1:
        break;
    };

    prevState = signalAB;

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

void updateYaw(void)
{
    // convert yaw to a range between 0 to 360 degrees from gpio value
    yaw_pos.degree = ((yaw * 360) / 448) % 360; // degree precision
    yaw_pos.sub_degree = ((yaw * 8) % 360) % 10; // sub degree precision

    // constrain yaw between -180 to 180 degrees
    if((yaw_pos.degree > 180))
    {
        yaw_pos.degree = yaw_pos.degree - 360;
    }
    else if(yaw_pos.degree <= -180)
    {
        yaw_pos.degree = yaw_pos.degree + 360;
    }
}

YawPosition getYaw(void)
{
    return yaw_pos;
}
