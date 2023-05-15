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

static int16_t yaw;
static YawPosition yaw_pos;

static bool yawRef = false;

static int16_t desired_yaw = 0;

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

    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_RISING_EDGE);

    GPIOIntRegister(GPIO_PORTC_BASE, YawRefIntHandler);

    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);

    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4);
}

void YawRefIntHandler(void)
{
    int signalRef= GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);

    if(!signalRef)
        yawRef = false;
    else
        yawRef = true;

    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);
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
    if((yaw_pos.degree > MAX_YAW))
    {
        yaw_pos.degree = yaw_pos.degree - FULL_ROTATION;
    }
    else if(yaw_pos.degree <= MIN_YAW)
    {
        yaw_pos.degree = yaw_pos.degree + FULL_ROTATION;
    }
}

YawPosition getYaw(void)
{
    return yaw_pos;
}


void incrementYaw(void)
{
    desired_yaw += YAW_INCREMENT;

    if((desired_yaw > MAX_YAW))
    {
        desired_yaw -= FULL_ROTATION;
    }
    else if(desired_yaw <= MIN_YAW)
    {
        desired_yaw += FULL_ROTATION;
    }
}

void decrementYaw(void)
{
    desired_yaw -= YAW_INCREMENT;

    if((desired_yaw > MAX_YAW))
    {
        desired_yaw -= FULL_ROTATION;
    }
    else if(desired_yaw <= MIN_YAW)
    {
        desired_yaw += FULL_ROTATION;
    }
}

int16_t getDesiredYaw(void)
{
    return desired_yaw;
}

int16_t getYawError(void)
{
    int16_t degree_error = getDesiredYaw() - getYaw().degree;

    if((degree_error > MAX_YAW))
    {
        degree_error -= FULL_ROTATION;
    }
    else if(degree_error <= MIN_YAW)
    {
        degree_error += FULL_ROTATION;
    }

    return degree_error;
}

bool getYawRef()
{
    return yawRef;
}
