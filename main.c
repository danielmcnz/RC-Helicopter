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

#include "buttons4.h"
#include "circBufT.h"
#include "yaw.h"
#include "altitude.h"
#include "display.h"
#include "clock.h"
#include "kernel.h"
#include "rotors.h"
#include "control.h"
#include "uart.h"
#include "switch.h"
#include "heliState.h"
#include "reset.h"

#define SAMPLE_RATE_HZ 250

#define ALTITUDE_UPDATE_FREQUENCY 150
#define YAW_UPDATE_FREQUENCY 150
#define INPUT_UPDATE_FREQUENCY 10
#define DISPLAY_UPDATE_FREQUENCY 4
#define CONTROL_UPDATE_FREQUENCY 20
#define UART_UPDATE_FREQUENCY 4

#define ALTITUDE_PRIORITY 1
#define YAW_PRIORITY 1
#define INPUT_PRIORITY 2
#define DISPLAY_PRIORITY 3
#define CONTROL_PRIORITY 0
#define UART_PRIORITY 3

void initialize(void)
{
    initClock ();
    initKernel(SAMPLE_RATE_HZ);
    initAltitude();
    initDisplay ();
    initRotors();
    initControl(CONTROL_UPDATE_FREQUENCY);
    initUART();
    initReset();
    initButtons();
    initSwitch();
    initYaw();

    // Enable interrupts to the processor.
    IntMasterEnable();
}

static bool test = false;
static bool test2 = false;

void updateInput(void)
{
    updateButtons();
    updateSwitch();

    if(getHeliState() == FLYING)
    {
        if(checkButton(UP) == PUSHED)
        {
            incrementAltitude();
        }
        else if(checkButton(DOWN) == PUSHED)
        {
            decrementAltitude();
        }
        else if(checkButton(LEFT) == PUSHED)
        {
            decrementYaw();
        }
        else if(checkButton(RIGHT) == PUSHED)
        {
            incrementYaw();
        }
    }

    if(checkSwitch() == SWITCH_UP && getHeliState() == LANDED)
    {
        setHeliState(TAKING_OFF);
        startRotors();
    }
    if(checkSwitch() == SWITCH_DOWN)
        {
            test = true;
            if(getHeliState() == FLYING)
            {
                test2 = true;
                setHeliState(LANDING);
            }
        }
}

int main(void)
{
    initialize();

    kernelRegisterTask(SAMPLE_RATE_HZ / ALTITUDE_UPDATE_FREQUENCY, &updateAltitude, ALTITUDE_PRIORITY);
    kernelRegisterTask(SAMPLE_RATE_HZ / YAW_UPDATE_FREQUENCY, &updateYaw, YAW_PRIORITY);
    kernelRegisterTask(SAMPLE_RATE_HZ / INPUT_UPDATE_FREQUENCY, &updateInput, INPUT_PRIORITY);
    kernelRegisterTask(SAMPLE_RATE_HZ / CONTROL_UPDATE_FREQUENCY, &updateControl, CONTROL_PRIORITY);
    kernelRegisterTask(SAMPLE_RATE_HZ / DISPLAY_UPDATE_FREQUENCY, &updateDisplay, DISPLAY_PRIORITY);
    kernelRegisterTask(SAMPLE_RATE_HZ / UART_UPDATE_FREQUENCY, &sendStatus, UART_PRIORITY);

    kernelPrioritise();

    while (1)
    {
        kernelRun();
    }
}
