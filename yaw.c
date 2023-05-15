/*
 * Yaw.cpp
 *
 *  Created on: 3/05/2023
 *      Author: dmc270
 */

#include "yaw.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"

static int16_t yaw;
static YawPosition yaw_pos;

static bool yawRef = false;

static int16_t desired_yaw = 0;
static int16_t degree_error = 0;

void YawIntHandler(void);
void YawRefIntHandler(void);

void initYaw(void)
{
    // initialize yaw A and B

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);

    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // initialize yaw reference

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);

    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA,
                GPIO_PIN_TYPE_STD_WPU);

    GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);

    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);

    GPIOIntRegister(GPIO_PORTC_BASE, YawRefIntHandler);

    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);

    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4);

    IntEnable(INT_GPIOC);
}

void YawRefIntHandler(void)
{
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);

    // bool signalRef = GPIO_PIN_4 && GPIOIntStatus(GPIO_PORTC_BASE, false);

    // if(signalRef)
    yawRef = true;
    //else
        //yawRef = false;

}

void YawIntHandler(void)
{
    static int prevState = INVALID;

    int signalAB = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    switch(prevState)
    {
    case LO_LO:
        signalAB == HI_LO ? yaw++ : yaw--;
        break;
    case HI_LO:
        signalAB == HI_HI ? yaw++ : yaw--;
        break;
    case LO_HI:
        signalAB == LO_LO ? yaw++ : yaw--;
        break;
    case HI_HI:
        signalAB == LO_HI ? yaw++ : yaw--;
        break;
    case INVALID:
        break;
    };

    prevState = signalAB;

    // ensures yaw value does not overflow integer type
    if(yaw > N_SLOTS)
    {
        yaw -= N_SLOTS;
    } else if(yaw < -N_SLOTS)
    {
        yaw += N_SLOTS;
    }

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

void boundYawDeg(int16_t boundingVar)
{
    if((boundingVar > MAX_YAW))
    {
        boundingVar -= FULL_ROTATION;
    }
    else if(boundingVar <= MIN_YAW)
    {
        boundingVar += FULL_ROTATION;
    }
}

void updateYaw(void)
{
    // convert yaw to a range between 0 to 360 degrees from gpio value
    yaw_pos.degree = ((yaw * FULL_ROTATION) / N_SLOTS) % FULL_ROTATION; // degree precision
    // yaw_pos.sub_degree = ((yaw * 8) % FULL_ROTATION) % 10; // sub degree precision
    yaw_pos.sub_degree = ((yaw * DECIMAL_PLACES * FULL_ROTATION) / N_SLOTS) % DECIMAL_PLACES;

    // constrain yaw sub precision to be positive as sign is already handled by degree calculation
    if(yaw_pos.sub_degree < 0)
        yaw_pos.sub_degree = -yaw_pos.sub_degree;

    // constrain yaw between -180 to 180 degrees
    boundYawDeg(yaw_pos.degree);
}

YawPosition getYaw(void)
{
    return yaw_pos;
}


void incrementYaw(void)
{
    desired_yaw += YAW_INCREMENT;

    boundYawDeg(desired_yaw);
}

void decrementYaw(void)
{
    desired_yaw -= YAW_INCREMENT;

    boundYawDeg(desired_yaw);
}

int16_t getDesiredYaw(void)
{
    return desired_yaw;
}

int16_t getYawError(void)
{
    degree_error = getDesiredYaw() - getYaw().degree;

    boundYawDeg(degree_error);

    return degree_error;
}

bool getYawRef()
{
    return yawRef;
}
