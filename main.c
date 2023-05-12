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

#define SAMPLE_RATE_HZ                  60


// ----------------------------------- //
// PWM specific macros to be moved
// ----------------------------------- //

#define PWM_RATE_HZ                     200
#define PWM_START_PC                    10
#define PWN_DIVIDER_CODE                SYSCTL_PWMDIV_2
#define PWM_DIVIDER                     2


#define PWM_MAIN_ROTOR_PERIPH_PWM       SYSCTL_PERIPH_PWM0
#define PWM_MAIN_ROTOR_PERIPH_GPIO      SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_ROTOR_GPIO_CONFIG      GPIO_PC5_M0PWM7
#define PWM_MAIN_ROTOR_GPIO_BASE        GPIO_PORTC_BASE
#define PWM_MAIN_ROTOR_GPIO_PIN         GPIO_PIN_5
#define PWM_MAIN_ROTOR_BASE             PWM0_BASE
#define PWM_MAIN_ROTOR_GEN              PWM_GEN_3

#define PWM_SECONDARY_ROTOR_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_SECONDARY_ROTOR_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_SECONDARY_ROTOR_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_SECONDARY_ROTOR_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_SECONDARY_ROTOR_GPIO_PIN    GPIO_PIN_1
#define PWM_SECONDARY_ROTOR_BASE        PWM1_BASE
#define PWM_SECONDARY_ROTOR_GEN         PWM_GEN_2

// ----------------------------------- //
// End of PWM specific macros
// ----------------------------------- //


// ----------------------------------- //
// PWM specific functions to be moved
// ----------------------------------- //

void initialiseMainRotorPWM(void)
{
    SysCtlPeripheralEnable(PWM_MAIN_ROTOR_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_ROTOR_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_ROTOR_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_ROTOR_GPIO_BASE, PWM_MAIN_ROTOR_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_GEN, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_GEN, ui32Period);

    // SetPWM()
    // PWMGenEnable()
    // PWMOutputState()

    // used for setting rotor PWM, move to another function

    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / PWM_RATE_HZ;
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM, ui32Period * PWM_START_PC / 100);
}

void initialiseSecondaryRotorPWM(void)
{
    SysCtlPeripheralEnable(PWM_SECONDARY_ROTOR_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_SECONDARY_ROTOR_PERIPH_GPIO);

    GPIOPinConfigure(PWM_SECONDARY_ROTOR_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_SECONDARY_ROTOR_GPIO_BASE, PWM_SECONDARY_ROTOR_GPIO_PIN);

    PWMGenConfigure(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_GEN, PWM_GEN_MODE_UP_DOWM | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_GEN, ui32Period);

    // SetPWM()
    // PWMGenEnable()
    // PWMOutputState()

    // used for setting rotor PWM, move to another function

    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / PWM_RATE_HZ;
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM, ui32Period * PWM_START_PC / 100);
}

// ----------------------------------- //
// End of PWM specific functions
// ----------------------------------- //

void initialize(void)
{
    initClock ();
    initKernel();
    initAltitude();
    initButtons();
    initDisplay ();
    initYaw();

    SysCtlPWMClockSet(PWM_DIVIDER_CODE); // set PWM clock rate to 10 MHz (80Hz min rate) via pre-scale PWM_DIVIDER_CODE

    initialisePWM();

    // Enable interrupts to the processor.
    IntMasterEnable();
}

void update(void)
{
    static uint32_t display_timer = 0;

    updateYaw();
    updateAltitude();

    // delay between each display update as not nessecary to update display every tick
    if(display_timer >= DISPLAY_DELAY)
    {
        updateScreenState();

        updateDisplay(getMeanAltitude(), g_ulSampCnt, getAltitudePerc(), getYaw());

        // resets the initial initial altitude when switching between display screens
        if(isLeftButtonPushed())
           setInitAltitude(getMeanAltitude());

        display_timer = 0;
    }

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
