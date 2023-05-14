/*
 * Altitude.cpp
 *
 *  Created on: 3/05/2023
 *      Author: dmc270
 */

#include "altitude.h"

#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"

static circBuf_t g_inBuffer; // Buffer of size BUF_SIZE integers (sample values)
static int32_t buffer_sum; // sum of the values in the circular buffer
static int32_t init_mean_altitude; // initial altitude value
static uint32_t mean_altitude;
static int16_t percentage_val;
static uint16_t desired_altitude;

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void altitudeIntHandler(void)
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

void _initADC(void)
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
    ADCIntRegister (ADC0_BASE, 3, altitudeIntHandler);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);

    ADCProcessorTrigger(ADC0_BASE, 3);
}

void initAltitude(void)
{
    _initADC();

    initCircBuf (&g_inBuffer, BUF_SIZE);

    SysCtlDelay(10 * (SysCtlClockGet() / 60));

    // reads the sum of the buffer in the first pass after short initial delay
    buffer_sum = 0;
    int i = 0;
    for (; i < BUF_SIZE; i++)
        buffer_sum = buffer_sum + readCircBuf (&g_inBuffer);

    init_mean_altitude = (2 * buffer_sum + BUF_SIZE) / 2 / BUF_SIZE; // mean altitude

    desired_altitude = getAltitudePerc();
}

void updateAltitude(void)
{
    //
    // Background task: calculate the (approximate) mean of the values in the
    // circular buffer and display it, together with the sample number.

    buffer_sum = 0;
    int i = 0;
    for (; i < BUF_SIZE; i++)
        buffer_sum = buffer_sum + readCircBuf (&g_inBuffer);

    // Calculate and display the rounded mean of the buffer contents
    mean_altitude = (2 * buffer_sum + BUF_SIZE) / 2 / BUF_SIZE;

    if (init_mean_altitude >= mean_altitude)
    {
        percentage_val =  (PERCENT * (int16_t)(init_mean_altitude - mean_altitude)) / ALTITUDE_VOLTAGE_RANGE;
    }
    else
    {
        percentage_val = - (PERCENT * (int16_t)(mean_altitude - init_mean_altitude)) / ALTITUDE_VOLTAGE_RANGE;
    }
}

void setInitAltitude(uint32_t mean_altitude)
{
    init_mean_altitude = mean_altitude;
}

int16_t getAltitudePerc(void)
{
    return percentage_val;
}


uint32_t getMeanAltitude(void)
{
    return mean_altitude;
}

void incrementAltitude(void)
{
    desired_altitude += ALTITUDE_INCREMENT;
    if (desired_altitude > 100)
    {
        desired_altitude = 100;
    }
}

void decrementAltitude(void)
{

    if (((int16_t)(desired_altitude) - ALTITUDE_INCREMENT) < 0)
    {
        desired_altitude = 0;
    }
    else
    {
        desired_altitude -= ALTITUDE_INCREMENT;
    }
}

uint16_t getDesiredAltitude(void)
{
    return desired_altitude;
}

int16_t getAltitudeError(void)
{
    return getDesiredAltitude() - getAltitudePerc();
}
