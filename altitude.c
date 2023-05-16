//**********************************************************
// File: altitude.c
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Handles reading and calculating altitude and determining
// the height of the helicopter
//
//**********************************************************

#include "altitude.h"

#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"

//**********************************************************
// constants
//**********************************************************

#define ALTITUDE_VOLTAGE_RANGE 1000
#define PERCENT 100
#define ALTITUDE_INCREMENT 10

// altitude constraints
#define MAX_ALTITUDE 100
#define MIN_ALTITUDE 0

// buffer size of the circular buffer used to read altitude values
#define BUF_SIZE 10

// used for the delay period before filling the initial altitude circular buffer
#define DELAY_DIVIDER 6

//**********************************************************
// static variables
//**********************************************************

// Buffer of size BUF_SIZE integers (sample values)
static circBuf_t g_inBuffer; 

// sum of the values in the circular buffer
static int32_t buffer_sum; 

// initial mean altitude, averaged over 10 intital altitude samples
static int32_t init_mean_altitude; 

// mean altitude averaged over the last 10 alitude readings
static uint32_t mean_altitude; 

// altitude as a percentage
static int16_t altitude_percentage = 0;

// the altitude the helicopter should move to or currently be at
static int16_t desired_altitude = 0;

//*****************************************************************************
// altitudeIntHandler: handler for the ADC conversion complete interrupt for altitude,
// writes to the circular buffer
//*****************************************************************************
void altitudeIntHandler(void)
{
    uint32_t ulValue;

    // Get the single sample from ADC0.
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);

    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);

    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

//**********************************************************
// _initADC: initialize the ADC to read the altitude from the helicopter
//**********************************************************
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
}

//**********************************************************
// initAltitude: initialize the rotors for PWM output
//**********************************************************
void initAltitude(void)
{
    _initADC();

    initCircBuf (&g_inBuffer, BUF_SIZE);

    SysCtlDelay((SysCtlClockGet() / DELAY_DIVIDER));

    // reads the sum of the buffer in the first pass after short initial delay
    buffer_sum = 0;
    int i = 0;
    for (; i < BUF_SIZE; i++)
        buffer_sum = buffer_sum + readCircBuf (&g_inBuffer);

    init_mean_altitude = (2 * buffer_sum + BUF_SIZE) / 2 / BUF_SIZE; // mean altitude
}

// **********************************************************
// updateAltitude: updates the mean altitude value by reading the ADC
// and calculating it as a percentage
// **********************************************************
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
        altitude_percentage =  (PERCENT * (int16_t)(init_mean_altitude - mean_altitude)) / ALTITUDE_VOLTAGE_RANGE;
    }
    else
    {
        altitude_percentage = - (PERCENT * (int16_t)(mean_altitude - init_mean_altitude)) / ALTITUDE_VOLTAGE_RANGE;
    }
}

//**********************************************************
// setInitAltitude: resets the initial mean altitude to the given value
//**********************************************************
void setInitAltitude(uint32_t mean_altitude)
{
    init_mean_altitude = mean_altitude;
}

//**********************************************************
// setDesiredAltitude: sets the desired altitude to the given parameter
//**********************************************************
void setDesiredAltitude(int16_t desired)
{
    desired_altitude = desired;
}

//**********************************************************
// getAltitudePerc: returns the altitude as a percentage
//**********************************************************
int16_t getAltitudePerc(void)
{
    return altitude_percentage;
}

//**********************************************************
// getMeanAltitude: returns the mean altitude
//**********************************************************
uint32_t getMeanAltitude(void)
{
    return mean_altitude;
}

//**********************************************************
// incrementAltitude: manually increases the desired altitude
//**********************************************************
void incrementAltitude(void)
{
    desired_altitude += ALTITUDE_INCREMENT;
    if (desired_altitude > MAX_ALTITUDE)
    {
        desired_altitude = MAX_ALTITUDE;
    }
}

//**********************************************************
// decrementAltitude: manually decreases the desired altitude
//**********************************************************
void decrementAltitude(void)
{

    if ((desired_altitude - ALTITUDE_INCREMENT) < MIN_ALTITUDE)
    {
        desired_altitude = MIN_ALTITUDE;
    }
    else
    {
        desired_altitude -= ALTITUDE_INCREMENT;
    }
}

//**********************************************************
// getDesiredAltitude: returns the desired altitude
//**********************************************************
uint16_t getDesiredAltitude(void)
{
    return desired_altitude;
}

//**********************************************************
// getAltitudeError: returns the altitude error 
// (desired - current) as a percentage
//**********************************************************
int16_t getAltitudeError(void)
{
    return getDesiredAltitude() - getAltitudePerc();
}

//**********************************************************
// getInitAltitude: returns the initial mean altitude
//**********************************************************
int32_t getInitAltitude(void)
{
    return init_mean_altitude;
}