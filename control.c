#include "control.h"

static uint8_t  control_update_freq; // Check back on this <----------------

static uint32_t previous_altitude_error = 0;
static uint32_t sum_altitude_error = 0;

static uint32_t previous_yaw_error = 0;
static uint32_t sum_yaw_error = 0;

void calculateAltitudeControl(void);
void calculateYawControl(void);

void initControl(void)
{

}


void updateControl(void)
{
    calculateAltitudeControl();
    calculateYawControl();
}

void calculateAltitudeControl(void)
{
    uint32_t error = getAltitudeError();

    uint32_t derivative_error = (error - previous_altitude_error) * control_update_freq;
    
    uint32_t temp_errror_sum = sum_altitude_error += error;

    uint32_t control_output = ALTITUDE_KP * error + ALTITUDE_KD * derivative_error + ALTITUDE_KI * sum_altitude_error;

    if (control_output <= PWM_MAX_DUTY_CYCLE && control_output >= PWM_MIN_DUTY_CYCLE)
    {
        sum_altitude_error += temp_errror_sum;
    }

    previous_altitude_error = error;

    configureMainRotor(control_output);

}

void calculateYawControl(void)
{
    uint32_t error = getYawError();

    uint32_t derivative_error = (error - previous_yaw_error) * control_update_freq;
    
    uint32_t temp_errror_sum = sum_yaw_error + error / control_update_freq;

    uint32_t control_output = YAW_KP * error + YAW_KD * derivative_error + YAW_KI * sum_yaw_error;

    if (control_output <= PWM_MAX_DUTY_CYCLE && control_output >= PWM_MIN_DUTY_CYCLE)
    {
        sum_yaw_error += temp_errror_sum;
    }

    previous_yaw_error = error;

    configureSecondaryRotor(control_output);

}