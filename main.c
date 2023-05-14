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
#include "uart.h"

#define SAMPLE_RATE_HZ 60

#define ALTITUDE_UPDATE_FREQUENCY 25
#define YAW_UPDATE_FREQUENCY 25
#define INPUT_UPDATE_FREQUENCY 2
#define DISPLAY_UPDATE_FREQUENCY 100
#define CONTROL_UPDATE_FREQUENCY 10
#define UART_UPDATE_FREQUENCY 100

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
    initButtons();
    initDisplay ();
    initYaw();
    initRotors();
    initControl(CONTROL_UPDATE_FREQUENCY);
    initUART();

    startRotors();

    // Enable interrupts to the processor.
    IntMasterEnable();
}

void updateInput(void)
{
    static uint8_t button_state;

    updateButtons();

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

int main(void)
{
    initialize();

    kernelRegisterTask(ALTITUDE_UPDATE_FREQUENCY, &updateYaw, ALTITUDE_PRIORITY);
    kernelRegisterTask(YAW_UPDATE_FREQUENCY, &updateAltitude, YAW_PRIORITY);
    kernelRegisterTask(INPUT_UPDATE_FREQUENCY, &updateInput, INPUT_PRIORITY);
    kernelRegisterTask(CONTROL_UPDATE_FREQUENCY, &updateControl, CONTROL_PRIORITY);
    kernelRegisterTask(DISPLAY_UPDATE_FREQUENCY, &updateDisplay, DISPLAY_PRIORITY);
    kernelRegisterTask(UART_UPDATE_FREQUENCY, &sendStatus, UART_PRIORITY);

    kernelPrioritise();

    while (1)
    {
        kernelRun();
    }
}
