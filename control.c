#include "control.h"

#include "rotors.h"
#include "altitude.h"
#include "yaw.h"
#include "heliState.h"

#define CONTROL_DIVISOR 1000

static uint8_t  control_update_freq;

static int32_t previous_altitude_error = 0;
static int32_t sum_altitude_error = 0;

static int16_t previous_yaw_error = 0;
static int32_t sum_yaw_error = 0;


void calculateAltitudeControl(void);
void calculateYawControl(void);

void initControl(uint8_t update_freq)
{
    control_update_freq = update_freq;
}

void updateControl(void)
{
    if(getHeliState() == TAKING_OFF)
    {
        // go to hover and rotate to known reference point

        configureMainRotor(50);

        configureSecondaryRotor(60);


        setHeliState(FLYING);
        // if(getYawRef())
        // {
        //     stopRotors();
        //     // setHeliState(FLYING);
        // }
    }
    else if(getHeliState() == LANDING)
    {
        // go to hover and rotate to known reference point, then land
    }
    else if(getHeliState() == FLYING)
    {
        calculateAltitudeControl();
        calculateYawControl();
    }
    else
    {
        stopRotors();
    }
}

void calculateAltitudeControl(void)
{
    int16_t error = getAltitudeError();

    int32_t proporional_error = error;

    int32_t derivative_error = (error - previous_altitude_error);
    previous_altitude_error = error;

    int32_t intergral_error_sum = sum_altitude_error + (error);

    int32_t control_output = (ALTITUDE_KP * proporional_error) + (ALTITUDE_KD * derivative_error) + (ALTITUDE_KI * intergral_error_sum);
    control_output /= CONTROL_DIVISOR;

    if ((control_output <= PWM_MAX_DUTY_CYCLE) && (control_output >= PWM_MIN_DUTY_CYCLE))
    {
        sum_altitude_error = intergral_error_sum;
    }

    configureMainRotor(50+control_output);
}

static int16_t error_temp_watch = 0;
static int16_t error_temp_watch_after = 0;

void calculateYawControl(void)
{
    bool direction_clock_wise = true;

    int32_t proporional;
    int32_t derivative;
    int32_t intergral;
    int32_t control_output;

    int16_t error = getYawError();
    
    int8_t duty_cycle;

    if (error < 0)
    {
        direction_clock_wise = false;
        error *= -1;
    }
    if (error > 180)
    {
        error = 360 - error;

        if (direction_clock_wise)
        {
            direction_clock_wise = false;
        }
        else
        {
            direction_clock_wise = true;
        }
    }
    if (!direction_clock_wise)
    {
        error *= -1;
    }

    proporional = error * YAW_KP;
    if (proporional > 10 * CONTROL_DIVISOR)
    {
        proporional = 10 * CONTROL_DIVISOR;
    }
    else if (proporional < -10 * CONTROL_DIVISOR)
    {
        proporional = -10 * CONTROL_DIVISOR;
    }

    derivative = (error - previous_yaw_error) * YAW_KD;
    previous_yaw_error = error;
    if (derivative > 10 * CONTROL_DIVISOR)
    {
        derivative = 10 * CONTROL_DIVISOR;
    }
    else if (derivative < -10 * CONTROL_DIVISOR)
    {
        derivative = -10 * CONTROL_DIVISOR;
    }

    intergral = (sum_yaw_error + error) * YAW_KI;
    if (intergral > 30 * CONTROL_DIVISOR)
    {
        intergral = 30 * CONTROL_DIVISOR;
    }
    else if (intergral < -30 * CONTROL_DIVISOR)
    {
        intergral = -30 * CONTROL_DIVISOR;
    }

    control_output = proporional + derivative + intergral;
    control_output /= CONTROL_DIVISOR;

    if ((control_output <= PWM_MAX_DUTY_CYCLE) && (control_output >= PWM_MIN_DUTY_CYCLE_TAIL))
    {
        sum_yaw_error = intergral;
    }

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

    error_temp_watch = duty_cycle;

    configureSecondaryRotor(duty_cycle);

}
