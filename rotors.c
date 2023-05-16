//**
// File: rotors.c
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Configures the helicopters main and tails rotors for PWM
//
//** 

#include "rotors.h"

#include "inc/hw_memmap.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

#include "heliState.h"

// **********************************************************
// constants
// **********************************************************

// PWM clock rate (processor clock / 2)
#define PWM_DIVIDER_CODE                SYSCTL_PWMDIV_2

// macros for main rotor
#define PWM_MAIN_ROTOR_FREQUENCY        250
#define PWM_MAIN_ROTOR_PERIPH_PWM       SYSCTL_PERIPH_PWM0
#define PWM_MAIN_ROTOR_PERIPH_GPIO      SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_ROTOR_GPIO_CONFIG      GPIO_PC5_M0PWM7
#define PWM_MAIN_ROTOR_GPIO_BASE        GPIO_PORTC_BASE
#define PWM_MAIN_ROTOR_GPIO_PIN         GPIO_PIN_5
#define PWM_MAIN_ROTOR_BASE             PWM0_BASE
#define PWM_MAIN_ROTOR_GEN              PWM_GEN_3
#define PWM_MAIN_ROTOR_OUT              PWM_OUT_7
#define PWM_MAIN_ROTOR_OUTBIT           PWM_OUT_7_BIT

// macros for secondary rotor
#define PWM_SECONDARY_ROTOR_FREQUENCY   250
#define PWM_SECONDARY_ROTOR_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_SECONDARY_ROTOR_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_SECONDARY_ROTOR_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_SECONDARY_ROTOR_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_SECONDARY_ROTOR_GPIO_PIN    GPIO_PIN_1
#define PWM_SECONDARY_ROTOR_BASE        PWM1_BASE
#define PWM_SECONDARY_ROTOR_GEN         PWM_GEN_2
#define PWM_SECONDARY_ROTOR_OUT         PWM_OUT_5
#define PWM_SECONDARY_ROTOR_OUTBIT      PWM_OUT_5_BIT

// **********************************************************
// forward declarations
// **********************************************************

static void _initMainRotor(void);
static void _initSecondaryRotor(void);

static void _startMainRotor(void);
static void _startSecondaryRotor(void);

static void _stopMainRotor(void);
static void _stopSecondaryRotor(void);

static uint8_t main_rotor_duty_cycle = 0;
static uint8_t secondary_rotor_duty_cycle = 0;

// **********************************************************
// initRotors: initialize the rotors for PWM output
// **********************************************************
void initRotors(void)
{
    // set PWM clock rate to 10 MHz (80Hz min rate) via pre-scale PWM_DIVIDER_CODE
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);

    _initMainRotor();
    _initSecondaryRotor();
}

// **********************************************************
// _initMainRotor: initialize the main rotor for PWM output
// PWM output initially disabled
// **********************************************************
static void _initMainRotor(void)
{
    // reset PWM and GPIO peripherals
    SysCtlPeripheralReset(PWM_MAIN_ROTOR_PERIPH_PWM);
    SysCtlPeripheralReset(PWM_MAIN_ROTOR_PERIPH_GPIO);

    // enable PWM and GPIO peripherals
    SysCtlPeripheralEnable(PWM_MAIN_ROTOR_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_ROTOR_PERIPH_GPIO);

    // ???
    GPIOPinConfigure(PWM_MAIN_ROTOR_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_ROTOR_GPIO_BASE, PWM_MAIN_ROTOR_GPIO_PIN);

    //configure PWM generator for count up down mode with no sync
    PWMGenConfigure(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_GEN, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    // start timers for main rotor generator
    PWMGenEnable(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_GEN);

    // disable output for main rotor
    PWMOutputState(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_OUTBIT, false);
}

// **********************************************************
// _initSecondaryRotor: initialize the tail rotor for PWM output
// PWM output initially disabled
// **********************************************************
static void _initSecondaryRotor(void)
{
    // reset PWM and GPIO peripherals
    SysCtlPeripheralReset(PWM_SECONDARY_ROTOR_PERIPH_PWM);
    SysCtlPeripheralReset(PWM_SECONDARY_ROTOR_PERIPH_GPIO);

    // enable PWM and GPIO peripherals
    SysCtlPeripheralEnable(PWM_SECONDARY_ROTOR_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_SECONDARY_ROTOR_PERIPH_GPIO);

    // ???
    GPIOPinConfigure(PWM_SECONDARY_ROTOR_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_SECONDARY_ROTOR_GPIO_BASE, PWM_SECONDARY_ROTOR_GPIO_PIN);

    //configure PWM generator for count up down mode with no sync
    PWMGenConfigure(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_GEN, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    // start timers for secondary rotor generator
    PWMGenEnable(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_GEN);

    // disable output for secondary rotor
    PWMOutputState(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_OUTBIT, false);
}

// **********************************************************
// configureMainRotor: sets the period and duty cycle for the main rotor, 
// bounded between the constraints if nessecary
// **********************************************************
void configureMainRotor(int32_t duty_cycle)
{
    // checks that duty cycle is between the min and max bounds
    if(duty_cycle > PWM_MAX_DUTY_CYCLE)
    {
        duty_cycle = PWM_MAX_DUTY_CYCLE;
    }
    else if (duty_cycle < PWM_MIN_DUTY_CYCLE)
    {
        duty_cycle = PWM_MIN_DUTY_CYCLE;
    }
    main_rotor_duty_cycle = (uint8_t)duty_cycle;

    // period = clock rate / 2 / rotor frequency
    uint32_t period = SysCtlClockGet() / PWM_DIVIDER / PWM_MAIN_ROTOR_FREQUENCY;

    // sets the period
    PWMGenPeriodSet(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_GEN, period);

    // set pulse width for main rotor PWM
    PWMPulseWidthSet(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_OUT, period * main_rotor_duty_cycle / 100);
}

// **********************************************************
// configureSecondaryRotor: sets the period and duty cycle for the 
// secondary rotor, bounded between the constraints if nessecary
// **********************************************************
void configureSecondaryRotor(uint8_t duty_cycle)
{
    secondary_rotor_duty_cycle = duty_cycle;

    // period = clock rate / 2 / rotor frequency
    uint32_t period = SysCtlClockGet() / PWM_DIVIDER / PWM_SECONDARY_ROTOR_FREQUENCY;

    // sets the period
    PWMGenPeriodSet(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_GEN, period);

    // set pulse width for secondary rotor PWM
    PWMPulseWidthSet(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_OUT, period * secondary_rotor_duty_cycle / 100);
}

// **********************************************************
// startRotors: enables PWM on the rotors
// **********************************************************
void startRotors(void)
{
    _startMainRotor();
    _startSecondaryRotor();
}

// **********************************************************
// _startMainRotor: enables PWM for the main rotor
// **********************************************************
static void _startMainRotor(void)
{
    PWMOutputState(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_OUTBIT, true);
}

// **********************************************************
// _startSecondaryRotor: enables PWM for the tail rotor
// **********************************************************
static void _startSecondaryRotor(void)
{
    PWMOutputState(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_OUTBIT, true);
}

// **********************************************************
// stopRotors: disables PWM on the rotors
// **********************************************************
void stopRotors(void)
{
    _stopMainRotor();
    _stopSecondaryRotor();
}

// **********************************************************
// stopRotors: disables PWM on the main rotor and resets the duty cycle
// **********************************************************
static void _stopMainRotor(void)
{
    PWMOutputState(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_OUTBIT, false);
    main_rotor_duty_cycle = 0;
}

// **********************************************************
// stopRotors: disables PWM on the tail rotor and resets the duty cycle
// **********************************************************
static void _stopSecondaryRotor(void)
{
    PWMOutputState(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_OUTBIT, false);
    secondary_rotor_duty_cycle = 0;
}

// **********************************************************
// getMainRotorDutyCycle: returns the duty cycle of the 
// main rotor of the helicopter
// **********************************************************
uint8_t getMainRotorDutyCycle(void)
{
    return main_rotor_duty_cycle;
}

// **********************************************************
// getSecondaryRotorDutyCycle: returns the duty cycle of the 
// tail rotor of the helicopter
// **********************************************************
uint8_t getSecondaryRotorDutyCycle(void)
{
    return secondary_rotor_duty_cycle;
}
