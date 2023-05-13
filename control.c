#include "control.h"

static uint8_t  control_update_freq;

static uint16_t previous_altitude_error = 0;
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
    calculateAltitudeControl();
    calculateYawControl();
}

void calculateAltitudeControl(void)
{
    uint16_t error = getAltitudeError();

    uint32_t proporional_error = error * 100;
    uint32_t derivative_error = (error - previous_yaw_error) * control_update_freq * 100;
    uint32_t intergral_error_sum = sum_yaw_error + (error * 100) / control_update_freq;

    uint32_t control_output = YAW_KP * proporional_error + YAW_KD * derivative_error + YAW_KI * intergral_error_sum;
    control_output /= 100;

    if (control_output <= PWM_MAX_DUTY_CYCLE && control_output >= PWM_MIN_DUTY_CYCLE)
    {
        sum_altitude_error += intergral_error_sum;
    }

    previous_altitude_error = error;

    configureMainRotor(control_output);

}

void calculateYawControl(void)
{
    uint16_t error = getYawError();

    uint32_t proporional_error = error * 100;
    uint32_t derivative_error = (error - previous_yaw_error) * control_update_freq * 100;
    uint32_t intergral_error_sum = sum_yaw_error + (error * 100) / control_update_freq;

    uint32_t control_output = YAW_KP * proporional_error + YAW_KD * derivative_error + YAW_KI * intergral_error_sum;
    control_output /= 100;

    if (control_output <= PWM_MAX_DUTY_CYCLE && control_output >= PWM_MIN_DUTY_CYCLE)
    {
        sum_yaw_error += intergral_error_sum;
    }

    previous_yaw_error = error;

    configureSecondaryRotor(control_output);

}
