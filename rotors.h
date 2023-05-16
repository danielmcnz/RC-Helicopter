//**
// File: rotors.h
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Configures the helicopters main and tails rotors for PWM
//
//** 

#ifndef ROTORS_H_
#define ROTORS_H_

#include <stdint.h>
#include <stdbool.h>

// divider for the rotor pulse width period
#define PWM_DIVIDER                         2

// default duty cycle values for initialization
#define DEFAULT_MAIN_ROTOR_DUTY_CYCLE       25
#define DEFAULT_SECONDARY_ROTOR_DUTY_CYCLE  25

// duty cycle bounds for rotor PWM
#define PWM_MAX_DUTY_CYCLE                  70
#define PWM_MIN_DUTY_CYCLE                  20
#define PWM_MIN_DUTY_CYCLE_TAIL             2

// **********************************************************
// initRotors: initialize the rotors for PWM output
// **********************************************************
void initRotors(void);

// **********************************************************
// configureMainRotor: sets the period and duty cycle for the main rotor, 
// bounded between the constraints if nessecary
// **********************************************************
void configureMainRotor(int32_t duty_cycle);

// **********************************************************
// configureSecondaryRotor: sets the period and duty cycle for the 
// secondary rotor, bounded between the constraints if nessecary
// **********************************************************
void configureSecondaryRotor(uint8_t duty_cycle);

// **********************************************************
// startRotors: enables PWM on the rotors
// **********************************************************
void startRotors(void);

// **********************************************************
// stopRotors: disables PWM on the rotors
// **********************************************************
void stopRotors(void);


// **********************************************************
// getMainRotorDutyCycle: returns the duty cycle of the 
// main rotor of the helicopter
// **********************************************************
uint8_t getMainRotorDutyCycle(void);

// **********************************************************
// getSecondaryRotorDutyCycle: returns the duty cycle of the 
// tail rotor of the helicopter
// **********************************************************
uint8_t getSecondaryRotorDutyCycle(void);

#endif /* ROTORS_H_ */
