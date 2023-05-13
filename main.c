//*****************************************************************************
//
// ADCdemo1.c - Simple interrupt driven program which samples with AIN0
//
// Author:  P.J. Bones  UCECE
// Last modified:   8.2.2018
//
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

#include "circBufT.h"
#include "buttons4.h"
#include "yaw.h"
#include "altitude.h"
#include "display.h"
#include "clock.h"
#include "kernel.h"
#include "rotors.h"

#define SAMPLE_RATE_HZ 60

void initialize(void)
{
    initClock ();
    initKernel(SAMPLE_RATE_HZ);
    initAltitude();
    initButtons();
    initDisplay ();
    initYaw();
    initRotors();

    // Enable interrupts to the processor.
    IntMasterEnable();
}

void updateInput(void)
{
    static uint8_t button_state;

    button_state = checkButton(UP);
    if(button_state == PUSHED)
    {
        incrementAltitude();
    }

    button_state = checkButton (DOWN);
    if(button_state == PUSHED)
    {
        decrementAltitude();
    }

    button_state = checkButton (LEFT);
    if(button_state == PUSHED)
    {
        decrementYaw();
    }

    button_state = checkButton (RIGHT);
    if(button_state == PUSHED)
    {
        incrementYaw();
    }
}

int
main(void)
{
    initialize();

    kernelRegisterTask(100, &updateYaw, 1);
    kernelRegisterTask(100, &updateAltitude, 1);
    kernelRegisterTask(50, &updateInput, 2);
    kernelRegisterTask(4, &updateDisplay, 3);

    // kernelPrioritise();

    while (1)
    {
        kernelRun();
    }
}
