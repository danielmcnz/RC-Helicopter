//**********************************************************
// File: control.c
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Calculates required duty cycle for main and secondary
// motors from the set errors and PID values.
//
//**********************************************************

#include "control.h"

#include "rotors.h"
#include "altitude.h"
#include "yaw.h"
#include "heliState.h"

//**********************************************************
// Constants
//**********************************************************

// This is the multiplier of the PID values to remove floating point arithmatic 
#define CONTROL_DIVISOR         1000

#define INTERGRAL_WINDUP_LIMITS 20
#define COUNTER_REACHED         400

#define INITIAL_YAW_CONTROL 25000
#define RISING_ALTITUDE_DIVISOR 100
#define YAW_CALIBRATION_SWEEP_SPEED 50
#define YAW_ERROR_TOLERANCE 5
#define MAX_HOVER_HEIGHT 2
#define MAX_YAW_DEG 180

//**********************************************************
// Static Variables
//**********************************************************

static int16_t yaw_error = 0;               // Units; Deg

static int32_t previous_altitude_error = 0; // Units; %
static int32_t sum_altitude_error = 0;      // Units; 0.01%

static int16_t previous_yaw_error = 0;      // Units; deg
static int32_t sum_yaw_error = 0;           // Units; 0.01 deg

//  Static variables used to find hovering point of heli
static uint32_t slow_rise = 2500;           // 0.01 dutycycle increse per tick
static uint16_t count = 0;
static uint8_t hover_point;                 
static bool hover_reached = false;

//**********************************************************
// Function declarations
//**********************************************************

void calculateAltitudeControl(void);
void calculateYawControl(void);
void resetLoop(void);

//**********************************************************
// updateControl: Checks the current state of the helicoper to overide the
// the desired yaw / altitude to either run the take off / 
// landing sequence
//**********************************************************
void updateControl(void)
{
    // Calculates the new duty cycle for each motor
    calculateAltitudeControl();
    calculateYawControl();

    // Checks if the current state of the heli is either taking off or landing
    if(getHeliState() == TAKING_OFF)
    {
        // Initialise function variables
        uint32_t mean_val = getMeanAltitude();
        int32_t init_val = getInitAltitude();

        if (!hover_point)
        {   
            // Costantly increase duty cycle by 0.01 until a change in mean altitude of 5 is recorded
            if (mean_val > (init_val - YAW_ERROR_TOLERANCE))
            {
                slow_rise++;
                configureMainRotor(slow_rise / RISING_ALTITUDE_DIVISOR);
            }
            else
            {
                hover_reached = true;
                hover_point = slow_rise / RISING_ALTITUDE_DIVISOR - MAX_HOVER_HEIGHT;
                configureMainRotor(hover_point);
            }
        }

        if (hover_reached)
        {
            // Configure tail motor to sweep along looking for yawRef 
            configureSecondaryRotor(YAW_CALIBRATION_SWEEP_SPEED);

            if (getYawRef())
            {
                // Set desired location to the current yawRef as yawRef is not the 0 point
                resetDesiredYaw();
                setHeliState(FLYING);
            }
        }
    }
    // Landing Sequence
    else if(getHeliState() == LANDING)
    {
        resetDesiredYaw();

        // +-5 tolerence for updating counter, gives enough time for resetDesiredYaw() to settle
        if(yaw_error < YAW_ERROR_TOLERANCE && yaw_error > -YAW_ERROR_TOLERANCE)
        {
            count++;
        }
        else
        {
            count = 0;
        }

        if (count >= COUNTER_REACHED)
        {
            setDesiredAltitude(0);

            if(getAltitudePerc() <= MAX_HOVER_HEIGHT)
            {
                setHeliState(LANDED);
                resetLoop();
                stopRotors();
            }
        }
    }
}

//**********************************************************
// calculateAltitudeControl: Updates the duty cycle for the main motor using the
// error returned from the altitude.c file
//**********************************************************
void calculateAltitudeControl(void)
{
    // Initialising function variables for each part of PID control
    int32_t proporional;
    int32_t derivative;
    int32_t intergral;
    int32_t control_output;

    int16_t error = getAltitudeError();

    int8_t duty_cycle;  // Bound to (PWM_MIN_DUTY_CYCLE->PWM_MAX_DUTY_CYCLE)

    proporional = error * ALTITUDE_KP;

    derivative = (error - previous_altitude_error) * ALTITUDE_KD;
    previous_altitude_error = error;

    // Intergral only sums after the other controls have decreased the error
    // helps to prevent the sum of intergral widning up
    if (error > -INTERGRAL_WINDUP_LIMITS && error < INTERGRAL_WINDUP_LIMITS)
    {
        sum_altitude_error += error;
    }
    intergral = sum_altitude_error * ALTITUDE_KI;

    control_output = (hover_point * CONTROL_DIVISOR) + proporional + intergral - derivative;
    control_output /= CONTROL_DIVISOR;

    // Bounds duty_cycle to with the PWM limits
    if(control_output > PWM_MAX_DUTY_CYCLE)
    {
        duty_cycle = PWM_MAX_DUTY_CYCLE;
    }
    else if (control_output < PWM_MIN_DUTY_CYCLE)
    {
        duty_cycle = PWM_MIN_DUTY_CYCLE;
    }
    else
    {
        duty_cycle = (uint8_t)control_output;
    }

    configureMainRotor(duty_cycle);
}

//**********************************************************
// calculateYawControl: Updates the duty cycle for the tail motor using the
// error returned from the yaw.c file
//**********************************************************
void calculateYawControl(void)
{
    // Initialises function variables

    bool direction_clock_wise = true;   // Fastest directing to reach desired_yaw

    // Variables for each part of PID control
    int32_t proporional;
    int32_t derivative;
    int32_t intergral;
    int32_t control_output;

    yaw_error = getYawError();
    
    int8_t duty_cycle;

    // Inverts direction and error to allow easier testing in next if statement
    if (yaw_error < 0)
    {
        direction_clock_wise = false;
        yaw_error *= -1;
    }
    // If error is greater than 180 it means its quicker to go the other direction and flip the error and direction
    if (yaw_error > MAX_YAW_DEG)
    {
        yaw_error = 2 * MAX_YAW_DEG - yaw_error;

        if (direction_clock_wise)
        {
            direction_clock_wise = false;
        }
        else
        {
            direction_clock_wise = true;
        }
    }
    // Reflips the error if the -ve error wasnt less the -180
    if (!direction_clock_wise)
    {
        yaw_error *= -1;
    }

    proporional = yaw_error * YAW_KP;

    derivative = (yaw_error - previous_yaw_error) * YAW_KD;
    previous_yaw_error = yaw_error;

    // Intergral only sums after the other controls have decreased the error
    // helps to prevent the sum of intergral widning up
    if (yaw_error > -INTERGRAL_WINDUP_LIMITS && yaw_error < INTERGRAL_WINDUP_LIMITS)
    {
        sum_yaw_error += yaw_error;
    }
    intergral = sum_yaw_error * YAW_KI;

    control_output = INITIAL_YAW_CONTROL + proporional + intergral - derivative;
    control_output /= CONTROL_DIVISOR;

    // Bounds duty_cycle to with the PWM limits for the tail motor
    if(control_output > PWM_MAX_DUTY_CYCLE)
    {
        duty_cycle = PWM_MAX_DUTY_CYCLE;
    }
    else if (control_output < PWM_MIN_DUTY_CYCLE_TAIL)
    {
        duty_cycle = PWM_MIN_DUTY_CYCLE_TAIL;
    }
    else
    {
        duty_cycle = (uint8_t)control_output;
    }

    configureSecondaryRotor(duty_cycle);

}

//**********************************************************
// resetLoop: When the state changes from landing to landed, reset
// some static variables to allow multiple smooth takeoffd
//**********************************************************
void resetLoop(void)
{
    previous_altitude_error = 0;
    sum_altitude_error = 0;

    previous_yaw_error = 0;
    sum_yaw_error = 0;
    count = 0;

    resetYawRef();
    resetDesiredYaw();
    setDesiredAltitude(0);
}
