#include "control.h"

#include "rotors.h"
#include "altitude.h"
#include "yaw.h"
#include "heliState.h"

#define CONTROL_DIVISOR 1000

static uint8_t  control_update_freq;

static int16_t yaw_error = 0;

static int32_t previous_altitude_error = 0;
static int32_t sum_altitude_error = 0;

static int16_t previous_yaw_error = 0;
static int32_t sum_yaw_error = 0;

//static uint8_t slow_descent = 35;
static uint32_t slow_rise = 2500;
static uint16_t count = 0;
static uint8_t hover_point;
static bool hover_reached = false;

void calculateAltitudeControl(void);
void calculateYawControl(void);

void initControl(uint8_t update_freq)
{
    control_update_freq = update_freq;
}

static bool hover_reached_blah;
static uint32_t mean_val_uuuu;
static uint16_t count_uu;

void resetLoop(void);

void updateControl(void)
{
    calculateAltitudeControl();
    calculateYawControl();

    if(getHeliState() == TAKING_OFF)
    {
        uint32_t mean_val = getMeanAltitude();
        int32_t init_val = getInitAltitude();

        mean_val_uuuu = mean_val;

        // go to hover and rotate to known reference point
        if (!hover_point)
        {
            if (mean_val > (init_val - 5))
            {
                slow_rise++;
                configureMainRotor(slow_rise/100);
            }
            else
            {
                hover_reached = true;
                hover_point = slow_rise/100 - 2;
                configureMainRotor(hover_point);
            }
        }

        hover_reached_blah = hover_reached;

        if (hover_reached)
        {
            configureSecondaryRotor(50);
//            resetDesiredYaw();


            if (getYawRef())
            {
                resetDesiredYaw();
                setHeliState(FLYING);
            }
        }
    }
    else if(getHeliState() == LANDING)
    {
        // go to hover and rotate to known reference point, then land
        resetDesiredYaw();

        if(yaw_error < 5 && yaw_error > -5)
        {
            count++;
        }
        else
        {
            count = 0;
        }

        count_uu = count;

        if (count >= 400)
        {
//            configureMainRotor(hover_point-2);
            setDesiredAltitude(0);

            if(getAltitudePerc() <= 2)
            {
                setHeliState(LANDED);
                resetLoop();
                stopRotors();
            }
        }
    }
//    else
//    {
//        stopRotors();
//    }
}

//void calculateAltitudeControl(void)
//{
//    int16_t error = getAltitudeError();
//
//    int32_t proporional_error = error;
//
//    int32_t derivative_error = (error - previous_altitude_error);
//    previous_altitude_error = error;
//
//    int32_t intergral_error_sum = sum_altitude_error + (error);
//
//    int32_t control_output = (ALTITUDE_KP * proporional_error) + (ALTITUDE_KD * derivative_error) + (ALTITUDE_KI * intergral_error_sum);
//    control_output /= CONTROL_DIVISOR;
//
//    if ((control_output <= PWM_MAX_DUTY_CYCLE) && (control_output >= PWM_MIN_DUTY_CYCLE))
//    {
//        sum_altitude_error = intergral_error_sum;
//    }
//
//    configureMainRotor(50+control_output);
//}

static int32_t proportional_a = 0;
static int32_t intergral_a = 0;
static int32_t derivative_a = 0;
static int32_t control_output_a = 0;
static int32_t duty_cycle_a = 0;

void calculateAltitudeControl(void)
{

    int32_t proporional;
    int32_t derivative;
    int32_t intergral;
    int32_t control_output;

    int16_t error = getAltitudeError();

    int8_t duty_cycle;

    proporional = error * ALTITUDE_KP;

    derivative = (error - previous_altitude_error) * ALTITUDE_KD;
    previous_altitude_error = error;

    if (error > -20 && error < 20)
    {
        sum_altitude_error += error;
    }
    intergral = sum_altitude_error * ALTITUDE_KI;

    // control_output = (slow_rise * CONTROL_DIVISOR) + proporional + intergral - derivative;
    control_output = (hover_point * CONTROL_DIVISOR) + proporional + intergral - derivative;
    control_output /= CONTROL_DIVISOR;

    derivative_a = derivative;
    intergral_a = intergral;
    proportional_a = proporional;
    control_output_a = control_output;

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

    duty_cycle_a = duty_cycle;

    configureMainRotor(duty_cycle);
}

static int32_t proportional_w = 0;
static int32_t intergral_w = 0;
static int32_t derivative_w = 0;
static int32_t control_output_w = 0;
static int32_t duty_cycle_w = 0;

void calculateYawControl(void)
{
    bool direction_clock_wise = true;

    int32_t proporional;
    int32_t derivative;
    int32_t intergral;
    int32_t control_output;

    yaw_error = getYawError();
    
    int8_t duty_cycle;

    if (yaw_error < 0)
    {
        direction_clock_wise = false;
        yaw_error *= -1;
    }
    if (yaw_error > 180)
    {
        yaw_error = 360 - yaw_error;

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
        yaw_error *= -1;
    }

    proporional = yaw_error * YAW_KP;

    derivative = (yaw_error - previous_yaw_error) * YAW_KD;
    previous_yaw_error = yaw_error;

    if (yaw_error > -20 && yaw_error < 20)
    {
//        if (error > 30)
//        {
//            sum_yaw_error += 30;
//        }
//        else if (error < -30)
//        {
//            sum_yaw_error -= 30;
//        }
//        else
//        {
//            sum_yaw_error += error;
//        }
        sum_yaw_error += yaw_error;
    }
    intergral = sum_yaw_error * YAW_KI;

    control_output = 25000 + proporional + intergral - derivative;
    control_output /= CONTROL_DIVISOR;

    derivative_w = derivative;
    intergral_w = intergral;
    proportional_w = proporional;
    control_output_w = control_output;

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

    duty_cycle_w = duty_cycle;

    configureSecondaryRotor(duty_cycle);

}

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
