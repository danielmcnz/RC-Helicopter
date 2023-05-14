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
#include "control.h"

#define SAMPLE_RATE_HZ 60

#define ALTITUDE_UPDATE_FREQUENCY 100
#define YAW_UPDATE_FREQUENCY 100
#define INPUT_UPDATE_FREQUENCY 250
#define DISPLAY_UPDATE_FREQUENCY 4
#define CONTROL_UPDATE_FREQUENCY 200

#define ALTITUDE_PRIORITY 1
#define YAW_PRIORITY 1
#define INPUT_PRIORITY 2
#define DISPLAY_PRIORITY 3
#define CONTROL_PRIORITY 0

void initialize(void)
{
    initClock ();
    initKernel(SAMPLE_RATE_HZ);
    initAltitude();
    initButtons();
    initDisplay ();
    initYaw();
    initRotors();
    initControl(CONTROL_UPDATE_FREQUENCY);

    startRotors();

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

    kernelRegisterTask(ALTITUDE_UPDATE_FREQUENCY, &updateYaw, ALTITUDE_PRIORITY);
    kernelRegisterTask(YAW_UPDATE_FREQUENCY, &updateAltitude, YAW_PRIORITY);
    kernelRegisterTask(INPUT_UPDATE_FREQUENCY, &updateInput, INPUT_PRIORITY);
    kernelRegisterTask(DISPLAY_UPDATE_FREQUENCY, &updateDisplay, DISPLAY_PRIORITY);
    kernelRegisterTask(CONTROL_UPDATE_FREQUENCY, &updateControl, CONTROL_PRIORITY);

    kernelPrioritise();

    while (1)
    {
        kernelRun();
    }
}
