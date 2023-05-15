#include "control.h"

#include "rotors.h"
#include "altitude.h"
#include "yaw.h"
#include "heliState.h"

#define CONTROL_DIVISOR 1000

static uint8_t  control_update_freq;

static uint32_t previous_altitude_error = 0;
static uint32_t sum_altitude_error = 0;

static uint16_t previous_yaw_error = 0;
static uint32_t sum_yaw_error = 0;

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

static int16_t error_temp_watch = 0;

void calculateAltitudeControl(void)
{
    int16_t error = getAltitudeError();

    error_temp_watch = error;

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

void calculateYawControl(void)
{
    int16_t error = getYawError();

    int32_t proporional_error = error;
    int32_t derivative_error = (error - previous_yaw_error);
    int32_t intergral_error_sum = sum_yaw_error + error;

    int16_t control_output = YAW_KP * proporional_error + YAW_KD * derivative_error + YAW_KI * intergral_error_sum;
    control_output /= CONTROL_DIVISOR;

    if (control_output <= PWM_MAX_DUTY_CYCLE && control_output >= PWM_MIN_DUTY_CYCLE)
    {
        sum_yaw_error = intergral_error_sum;
    }

    previous_yaw_error = error;

    configureSecondaryRotor(50+control_output);

}
