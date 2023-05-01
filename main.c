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
#include "circBufT.h"
#include "buttons4.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 10
#define SAMPLE_RATE_HZ 60

#define Range 1000

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;    // Counter for the interrupts
static int yaw = 0;

enum DisplayState {PERCENTAGE = 0, MEAN, OFF};

static uint8_t displayState = 0;

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
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void
ADCIntHandler(void)
{
    uint32_t ulValue;

    //
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    //
    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);
    //
    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

void
YawIntHandler(void)
{
    static int prevState = -1;

    int signalAB = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    switch(prevState)
    {
    case 0:
        signalAB == 1 ? yaw++ : yaw--;
        break;
    case 1:
        signalAB == 3 ? yaw++ : yaw--;
        break;
    case 2:
        signalAB == 0 ? yaw++ : yaw--;
        break;
    case 3:
        signalAB == 2 ? yaw++ : yaw--;
        break;
    case -1:
        break;
    };

    prevState = signalAB;

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
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

void
initADC (void)
{
    //
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

void
initYaw(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);

    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

void
initDisplay (void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}

void
ClearDisplayLine(uint8_t line)
{
    OLEDStringDraw ("                ", 0, line);
}

void
UpdateDisplay(uint32_t meanVal, uint32_t startingVal, uint32_t count, int16_t percentageVal, int32_t yawDeg)
{
    char string[17];  // 16 characters across the display
    char string1[17];

    switch(displayState)
    {
    case PERCENTAGE:
        OLEDStringDraw ("Percentage ADC  ", 0, 0);

        // Form a new string for the line.  The maximum width specified for the
        //  number field ensures it is displayed right justified.
        usnprintf (string, sizeof(string), "%6d", percentageVal);
        usnprintf (string1, sizeof(string1), "%6d", yawDeg);
        // Update line on display.
        OLEDStringDraw (string, 0, 1);

        OLEDStringDraw ("Yaw (Deg)       ", 0, 2);
        OLEDStringDraw (string1, 0, 3);

        break;
    case MEAN:
        OLEDStringDraw ("Mean ADC        ", 0, 0);

        usnprintf (string, sizeof(string), "%6d", meanVal);
        OLEDStringDraw (string, 1, 1);

        ClearDisplayLine(2);
        ClearDisplayLine(3);

        break;
    case OFF:
        ClearDisplayLine(0);
        ClearDisplayLine(1);
        ClearDisplayLine(2);
        ClearDisplayLine(3);
        break;
    }
}

void
UpdateInput(uint8_t *butState, uint32_t *startingVal, uint32_t meanVal)
{
    updateButtons ();

    *butState = checkButton (LEFT);
    if(*butState == PUSHED)
        *startingVal = meanVal;

    *butState = checkButton(UP);
    if(*butState == PUSHED)
    {
        displayState++;
        if(displayState > 2)
            displayState = 0;
    }
}

int
main(void)
{
    uint16_t i;
    int32_t sum;
    uint32_t meanVal;
    int16_t percentageVal;
    uint8_t butState;
    uint32_t startingVal;
    uint32_t display_delay = 100000000 / 64000; // how often display is updated.
    uint32_t display_timer = 0;
    int32_t yawDeg;


    initClock ();
    initADC ();
    initDisplay ();
    initButtons();
    initYaw();
    initCircBuf (&g_inBuffer, BUF_SIZE);


    //
    // Enable interrupts to the processor.
    IntMasterEnable();

    SysCtlDelay(10 * (SysCtlClockGet() / SAMPLE_RATE_HZ));

    sum = 0;
    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf (&g_inBuffer);

    startingVal = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;

    while (1)
    {
        //
        // Background task: calculate the (approximate) mean of the values in the
        // circular buffer and display it, together with the sample number.

        sum = 0;
        for (i = 0; i < BUF_SIZE; i++)
            sum = sum + readCircBuf (&g_inBuffer);

        // Calculate and display the rounded mean of the buffer contents
        meanVal = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;

        if (startingVal >= meanVal)
        {
            percentageVal =  (100 * (int16_t)(startingVal - meanVal)) / Range; // (100 * (startingVal - meanVal)) / startingVal;
        }
        else
        {
            percentageVal = - (100 * (int16_t)(meanVal - startingVal)) / Range; // - (int16_t)((100 * (meanVal - startingVal)) / meanVal);
        }

        yawDeg = ((yaw * 360) / 448) % 360;
        // yawDegDec = (yaw * 8) % 360 % 10;

        if((yawDeg > 180))
        {
            yawDeg = yawDeg - 360;
        }
        else if(yawDeg <= -180)
        {
            yawDeg = yawDeg + 360;
        }

        UpdateInput(&butState, &startingVal, meanVal);

        if(display_timer >= display_delay)
        {
            UpdateDisplay(meanVal, startingVal, g_ulSampCnt, percentageVal, yawDeg);
            display_timer = 0;
        }

         ++display_timer;

        // SysCtlDelay (SysCtlClockGet() / 6);  // Update display at ~ 4 Hz
    }
}
