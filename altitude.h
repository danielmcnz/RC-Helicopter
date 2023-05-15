/*
 * Altitude.h
 *
 *  Created on: 3/05/2023
 *      Author: dmc270
 */

#ifndef ALTITUDE_H_
#define ALTITUDE_H_

#include <stdint.h>
#include <stdbool.h>

#include "circBufT.h"

#define BUF_SIZE 10
#define ALTITUDE_VOLTAGE_RANGE 1000
#define PERCENT 100
#define ALTITUDE_INCREMENT 10

void altitudeIntHandler(void);

void _initADC(void);

void initAltitude(void);

void updateAltitude(void);

void setInitAltitude(uint32_t mean_altitude);

int16_t getAltitudePerc(void);
uint32_t getMeanAltitude(void);

void incrementAltitude(void);
void decrementAltitude(void);
uint16_t getDesiredAltitude(void);
int16_t getAltitudeError(void);
int32_t getInitAltitude(void);

void setDesiredAltitude(int16_t desired);

#endif /* ALTITUDE_H_ */
