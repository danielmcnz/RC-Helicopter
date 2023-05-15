/*
 * Yaw.cpp
 *
 *  Created on: 3/05/2023
 *      Author: dmc270
 */

#include "yaw.h"
#include "heliState.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"

static int16_t yaw;
static YawPosition yaw_pos;

static bool yawRef = false;

static int16_t desired_yaw = 0;

void YawRefIntHandler(void)
{
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);

    if (getHeliState() == TAKING_OFF)
    {
        yaw = 0;
        desired_yaw = 0;
        setHeliState(FLYING);
    }
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

    GPIOIntDisable(GPIO_PORTC_BASE, GPIO_PIN_4);

    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);

    GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);

    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA,
                GPIO_PIN_TYPE_STD_WPU);

    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);

    GPIOIntRegister(GPIO_PORTC_BASE, YawRefIntHandler);

    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4);

    // GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);

    IntEnable(INT_GPIOC);
}

int16_t boundYawDeg(int16_t boundingVar)
{
    if((boundingVar > MAX_YAW))
    {
        boundingVar -= FULL_ROTATION;
    }
    else if(boundingVar <= MIN_YAW)
    {
        boundingVar += FULL_ROTATION;
    }

    return boundingVar;
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
//    yaw_pos.degree = boundYawDeg(yaw_pos.degree);

    if((yaw_pos.degree > MAX_YAW))
    {
        yaw_pos.degree -= FULL_ROTATION;
    }
    else if(yaw_pos.degree <= MIN_YAW)
    {
        yaw_pos.degree += FULL_ROTATION;
    }
}

YawPosition getYaw(void)
{
    return yaw_pos;
}


void incrementYaw(void)
{
    desired_yaw += YAW_INCREMENT;

    desired_yaw = boundYawDeg(desired_yaw);
}

void decrementYaw(void)
{
    desired_yaw -= YAW_INCREMENT;

    desired_yaw = boundYawDeg(desired_yaw);
}

int16_t getDesiredYaw(void)
{
    return desired_yaw;
}

int16_t getYawError(void)
{
    int16_t degree_error = getYaw().degree - getDesiredYaw();

    return degree_error;
}

bool getYawRef()
{
    return yawRef;
}

void resetYaw(void)
{
    yaw = 0;
}