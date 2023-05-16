//**********************************************************
// File: altitude.h
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Handles reading and calculating altitude and determining
// the height of the helicopter
//
//**********************************************************

#ifndef ALTITUDE_H_
#define ALTITUDE_H_

#include <stdint.h>
#include <stdbool.h>

#include "circBufT.h"

//**********************************************************
// initAltitude: initialize the rotors for PWM output
//**********************************************************
void initAltitude(void);

//**********************************************************
// updateAltitude: updates the mean altitude value by reading the ADC
// and calculating it as a percentage
//**********************************************************
void updateAltitude(void);

//**********************************************************
// setInitAltitude: resets the initial mean altitude to the given value
//**********************************************************
void setInitAltitude(uint32_t mean_altitude);

//**********************************************************
// setDesiredAltitude: sets the desired altitude to the given parameter
//**********************************************************
void setDesiredAltitude(int16_t desired);

//**********************************************************
// getAltitudePerc: returns the altitude as a percentage
//**********************************************************
int16_t getAltitudePerc(void);

//**********************************************************
// getMeanAltitude: returns the mean altitude
//**********************************************************
uint32_t getMeanAltitude(void);

//**********************************************************
// incrementAltitude: manually increases the desired altitude
//**********************************************************
void incrementAltitude(void);

//**********************************************************
// decrementAltitude: manually decreases the desired altitude
//**********************************************************
void decrementAltitude(void);

//**********************************************************
// getDesiredAltitude: returns the desired altitude
//**********************************************************
uint16_t getDesiredAltitude(void);

//**********************************************************
// getAltitudeError: returns the altitude error 
// (desired - current) as a percentage
//**********************************************************
int16_t getAltitudeError(void);

//**********************************************************
// getInitAltitude: returns the initial mean altitude
//**********************************************************
int32_t getInitAltitude(void);

#endif /* ALTITUDE_H_ */
