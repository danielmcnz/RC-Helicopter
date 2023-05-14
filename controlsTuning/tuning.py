import matplotlib.pyplot as plt
import numpy as np

PID_SCALING_FACTOR = 100
CONTROL_DIVISOR = PID_SCALING_FACTOR * 10
PWM_MAX_DUTY_CYCLE = 98
PWM_MIN_DUTY_CYCLE = 2

control_update_freq = 250

sum_altitude_error = 0
previous_altitude_error = 0

Kp_A = 6 # altitude kp
Kd_A = 1 # altitude kd
Ki_A = 2 # altitude ki

def getAltitudeError():
    return 20


def calculate_altitude_controls():
    error = getAltitudeError()

    error_temp_watch = error

    proporional_error = error * PID_SCALING_FACTOR
    derivative_error = (error - previous_altitude_error) * control_update_freq * PID_SCALING_FACTOR
    intergral_error_sum = sum_altitude_error + (error * PID_SCALING_FACTOR) / control_update_freq

    control_output = (Kp_A * proporional_error) + (Kd_A * derivative_error) + (Ki_A * intergral_error_sum)
    control_output /= CONTROL_DIVISOR

    if ((control_output <= PWM_MAX_DUTY_CYCLE) and (control_output >= PWM_MIN_DUTY_CYCLE)):
        sum_altitude_error = intergral_error_sum

    previous_altitude_error = error

    return control_output



def main():


main()