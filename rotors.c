/*
 * rotors.c
 *
 *  Created on: 13/05/2023
 *      Author: dmc270
 */

#include "rotors.h"

#include "inc/hw_memmap.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

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

static void _initMainRotor(void);
static void _initSecondaryRotor(void);

static void _startMainRotor(void);
static void _startSecondaryRotor(void);

static void _stopMainRotor(void);
static void _stopSecondaryRotor(void);

static uint8_t main_rotor_duty_cycle = 0;
static uint8_t secondary_rotor_duty_cycle = 0;


static int16_t temp_var_watch = 0;

void initRotors(void)
{
    // set PWM clock rate to 10 MHz (80Hz min rate) via pre-scale PWM_DIVIDER_CODE
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);

    _initMainRotor();
    _initSecondaryRotor();
}

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

    configureMainRotor(DEFAULT_MAIN_ROTOR_DUTY_CYCLE);
}

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

    configureSecondaryRotor(DEFAULT_SECONDARY_ROTOR_DUTY_CYCLE);
}

void configureMainRotor(int16_t duty_cycle)
{
    temp_var_watch = duty_cycle;

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

void configureSecondaryRotor(int16_t duty_cycle)
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

    secondary_rotor_duty_cycle = (uint8_t)duty_cycle;

    // period = clock rate / 2 / rotor frequency
    uint32_t period = SysCtlClockGet() / PWM_DIVIDER / PWM_SECONDARY_ROTOR_FREQUENCY;

    // sets the period
    PWMGenPeriodSet(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_GEN, period);

    // set pulse width for secondary rotor PWM
    PWMPulseWidthSet(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_OUT, period * secondary_rotor_duty_cycle / 100);
}

void startRotors(void)
{
    _startMainRotor();
    _startSecondaryRotor();
}

static void _startMainRotor(void)
{
    PWMOutputState(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_OUTBIT, true);
}

static void _startSecondaryRotor(void)
{
    PWMOutputState(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_OUTBIT, true);
}

void stopRotors(void)
{
    _stopMainRotor();
    _stopSecondaryRotor();
}

static void _stopMainRotor(void)
{
    PWMOutputState(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_OUTBIT, false);
}

static void _stopSecondaryRotor(void)
{
    PWMOutputState(PWM_SECONDARY_ROTOR_BASE, PWM_SECONDARY_ROTOR_OUTBIT, false);
}

uint8_t getMainRotorDutyCycle(void)
{
    return main_rotor_duty_cycle;
}

uint8_t getSecondaryRotorDutyCycle(void)
{
    return secondary_rotor_duty_cycle;
}
