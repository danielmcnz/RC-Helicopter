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
    
    sum_altitude_error += error; // This might not be working (error * 100 / control_update_freq)

    uint32_t control_output = ALTITUDE_KP * error + ALTITUDE_KD * derivative_error + ALTITUDE_KI * sum_altitude_error;

    previous_altitude_error = error;

}

void calculateYawControl(void)
{
    uint32_t error = getYawError();

    uint32_t derivative_error = (error - previous_yaw_error) * control_update_freq;
    
    sum_yaw_error += error / control_update_freq; // Need to times by ~100ish to remove floating point

    uint32_t control_output = YAW_KP * error + YAW_KD * derivative_error + YAW_KI * sum_yaw_error;

    previous_yaw_error = error;
}