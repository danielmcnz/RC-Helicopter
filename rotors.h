/*
 * rotors.h
 *
 *  Created on: 13/05/2023
 *      Author: dmc270
 */

#ifndef ROTORS_H_
#define ROTORS_H_

#include <stdint.h>
#include <stdbool.h>

#define PWM_DIVIDER                         2

// default duty cycle values for initialization
#define DEFAULT_MAIN_ROTOR_DUTY_CYCLE       25
#define DEFAULT_SECONDARY_ROTOR_DUTY_CYCLE  25

// duty cycle bounds for rotor PWM
#define PWM_MAX_DUTY_CYCLE                  70 // Told to put as 70 in email as heli will turn off otherwise <----------------------------------------------------------------
#define PWM_MIN_DUTY_CYCLE                  20 // Just cause doesnt effect anything till ~50

// initialize the rotors for PWM output
void initRotors(void);

// sets the period and duty cycle for the main rotor, bounded between the constraints if nessecary
void configureMainRotor(int16_t duty_cycle);

// sets the period and duty cycle for the secondary rotor, bounded between the constraints if nessecary
void configureSecondaryRotor(int16_t duty_cycle);

// enables PWM on the rotors
void startRotors(void);

// disables PWM on the rotors
void stopRotors(void);

uint8_t getMainRotorDutyCycle(void);

uint8_t getSecondaryRotorDutyCycle(void);

#endif /* ROTORS_H_ */
