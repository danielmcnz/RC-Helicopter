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

//*****************************************************************************
// Constants
//*****************************************************************************
#define SAMPLE_RATE_HZ 60

//*****************************************************************************
// Global variables
//*****************************************************************************
static uint32_t g_ulSampCnt = 0;    // Counter for the interrupts

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC0_BASE, 3);
    g_ulSampCnt++;


}

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

void initialize(void)
{
    initClock ();
    initDisplay ();
    initButtons();
    initYaw();
    initAltitude();

    // Enable interrupts to the processor.
    IntMasterEnable();

    // set delay to fill circular buffer before reading initial values
    // SysCtlDelay(10 * (SysCtlClockGet() / SAMPLE_RATE_HZ));

}

void update(void)
{
    static uint32_t display_timer = 0;

    //updateYaw();
    //updateAltitude();

    // delay between each display update as not nessecary to update display every tick
    //if(display_timer >= DISPLAY_DELAY)
    //{
    OLEDStringDraw ("Percentage ADC  ", 0, 0);
        // updateDisplay(getMeanAltitude(), g_ulSampCnt, getAltitudePerc(), getYaw());

        // resets the initial initial altitude when switching between display screens
        //if(isLeftButtonPushed())
         //   setInitAltitude(getMeanAltitude());

        //display_timer = 0;
    //}

     ++display_timer;
}

int
main(void)
{
    initialize();

    while (1)
    {
        update();

        // SysCtlDelay (SysCtlClockGet() / 6);  // Update display at ~ 4 Hz
    }
}
