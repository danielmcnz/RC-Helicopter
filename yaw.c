//**********************************************************
// File: yaw.c
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Handles reading and calculating yaw and determining
// the rotation of the helicopter
//
//**********************************************************

#include "yaw.h"
#include "heliState.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"

//**********************************************************
// static variables
//**********************************************************

// Current yaw calcualted from a zeroed position, (+ve or -ve)
static int16_t yaw;

// Desired rotation (where it should be), used to calcuate the error and
// thus the PID cotnrol, can be incremented and decremented
static int16_t desired_yaw = 0;

// Struct containing the degree and sub degree for the rotation of the heli
static YawPosition yaw_pos;

// Boolean set true is the yawRef interupt handler is triggered during
// the taking off state
static bool yawRef = false;

//*****************************************************************************
// YawRefIntHandler: handler for the yawRef interupt, resets yaw and desired
// position to the current location at time of interupt
//*****************************************************************************
void YawRefIntHandler(void)
{
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);

    if (getHeliState() == TAKING_OFF)
    {
        yawRef = true;
        yaw = 0;
        desired_yaw = 0;
    }
}

//*****************************************************************************
// YawIntHandler: handler for the pin interrupts, used in the decoder to
// determine rotion and direction
//*****************************************************************************
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

    // bound yaw to +=448 (max yaw increments in 360 deg)
    if(yaw > N_SLOTS)
    {
        yaw -= N_SLOTS;
    } else if(yaw < -N_SLOTS)
    {
        yaw += N_SLOTS;
    }

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

//**********************************************************
// initYaw: initialize the YawRefIntHandler and
// YawIntHandler pin interupts
//**********************************************************
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

    IntEnable(INT_GPIOC);
}

//**********************************************************
// boundYawDeg: bounds an int16_t in degrees to between
// the MAX and MIN rotation (-179.9->180)
//**********************************************************
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

// **********************************************************
// updateYaw: updates the yaw_pos values by converting the
// yaw into degrees
// **********************************************************
void updateYaw(void)
{
    // convert yaw to a range between 0 to 360 degrees from gpio value
    // degree precision
    yaw_pos.degree = ((yaw * FULL_ROTATION) / N_SLOTS) % FULL_ROTATION;
    // subdegree precision
    yaw_pos.sub_degree = ((yaw * DECIMAL_PLACES * FULL_ROTATION) / N_SLOTS) % DECIMAL_PLACES;

    // constrain yaw sub precision to be positive as sign is already handled by degree calculation
    if(yaw_pos.sub_degree < 0)
        yaw_pos.sub_degree = -yaw_pos.sub_degree;

    if((yaw_pos.degree > MAX_YAW))
    {
        yaw_pos.degree -= FULL_ROTATION;
    }
    else if(yaw_pos.degree <= MIN_YAW)
    {
        yaw_pos.degree += FULL_ROTATION;
    }
}

//**********************************************************
// getYaw: Returns the stuct containing the current rotation
// in degrees (including subdegree)
//**********************************************************
YawPosition getYaw(void)
{
    return yaw_pos;
}

//**********************************************************
// resetDesiredYaw: Resets the desired yaw to 0
//**********************************************************
void resetDesiredYaw(void)
{
    desired_yaw = 0;
}

//**********************************************************
// incrementYaw: Increases the desired yaw by YAW_INCREMENT
// and then rebounds it back to +-180
//**********************************************************
void incrementYaw(void)
{
    desired_yaw += YAW_INCREMENT;

    desired_yaw = boundYawDeg(desired_yaw);
}

//**********************************************************
// decrementYaw: Decreases the desired yaw by YAW_INCREMENT
// and then rebounds it back to +-180
//**********************************************************
void decrementYaw(void)
{
    desired_yaw -= YAW_INCREMENT;

    desired_yaw = boundYawDeg(desired_yaw);
}

//**********************************************************
// getDesiredYaw: Returns the desired yaw value
//**********************************************************
int16_t getDesiredYaw(void)
{
    return desired_yaw;
}

//**********************************************************
// getYawError: Returns current yaw error, between the
// currrent position and the desired position
//**********************************************************
int16_t getYawError(void)
{
    int16_t degree_error = getYaw().degree - getDesiredYaw();

    return degree_error;
}

//**********************************************************
// getYawRef: Returns the falg signaling if the yawRef
// interupt has been triggered
//**********************************************************
bool getYawRef(void)
{
    return yawRef;
}

//**********************************************************
// resetYawRef: Reset the yawRef flag back to false, so
// taking off calcibration can be untertaken multiple times
//**********************************************************
void resetYawRef(void)
{
    yawRef = false;
}

//**********************************************************
// resetYaw: Resets the yaw, same as zeroing all rotation
// about where you call resetYaw()
//**********************************************************
void resetYaw(void)
{
    yaw = 0;
}
