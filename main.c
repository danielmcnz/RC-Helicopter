//**********************************************************
// File: main.c
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// main function for helicopter program
//
//**********************************************************

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

//**********************************************************
// constants
//**********************************************************

#define TESTING_ON_A_WORKING_HELI_RIG_FOR_THE_FIRST_TIME_FIVE_MINS_BEFORE_DEMONSTRATION 1

// kernel sample rate
#define SAMPLE_RATE_HZ 250

// kernel update frequency for each task
#define ALTITUDE_UPDATE_FREQUENCY 150
#define YAW_UPDATE_FREQUENCY 150
#define INPUT_UPDATE_FREQUENCY 50
#define DISPLAY_UPDATE_FREQUENCY 4
#define CONTROL_UPDATE_FREQUENCY 20
#define UART_UPDATE_FREQUENCY 4

// kernel priority for each task
#define ALTITUDE_PRIORITY 1
#define YAW_PRIORITY 1
#define INPUT_PRIORITY 2
#define DISPLAY_PRIORITY 3
#define CONTROL_PRIORITY 0
#define UART_PRIORITY 3

//**********************************************************
// initialize: initialization for all the background tasks
// and enables interrupts
//**********************************************************
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

//**********************************************************
// updateInput: handles button and switch input checks,
// controlling altitude and yaw of the helicopter and helicopter state
//**********************************************************
void updateInput(void)
{
    updateButtons();
    updateSwitch();

    // checks for button input, moving the helicopter if so,
    // provided the helicopter is in the flying state
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

    switch_state_t switch_1_input = checkSwitch();

    // handles switch input, checking for landing and 
    // taking off cases for the helicopter
    if(switch_1_input == SWITCH_UP && getHeliState() == LANDED)
    {
        setHeliState(TAKING_OFF);
        startRotors();
    }
    else if (switch_1_input == SWITCH_DOWN && getHeliState() == FLYING)
    {
        setHeliState(LANDING);
    }
}

//**********************************************************
// main: main loop of program, kernel setup registering all the
// background tasks and running them for the duration of the program
//**********************************************************
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

    while (TESTING_ON_A_WORKING_HELI_RIG_FOR_THE_FIRST_TIME_FIVE_MINS_BEFORE_DEMONSTRATION)
    {
        kernelRun();
    }
}
