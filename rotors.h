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
#define PWM_MAX_DUTY_CYCLE                  98
#define PWM_MIN_DUTY_CYCLE                  2

// initialize the rotors for PWM output
void initRotors(void);

// sets the period and duty cycle for the main rotor, bounded between the constraints if nessecary
void configureMainRotor(uint8_t duty_cycle);

// sets the period and duty cycle for the secondary rotor, bounded between the constraints if nessecary
void configureSecondaryRotor(uint8_t duty_cycle);

// enables PWM on the rotors
void startRotors(void);

// disables PWM on the rotors
void stopRotors(void);

uint8_t getMainRotorDutyCycle();

uint8_t getSecondaryRotorDutyCycle();

#endif /* ROTORS_H_ */
