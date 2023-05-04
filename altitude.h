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

void _initADC(void);

void initAltitude(void);

void altitudeIntHandler(void);

void updateAltitude(void);

void setInitAltitude(uint32_t mean_altitude);

int16_t getAltitudePerc(void);
uint32_t getMeanAltitude(void);

#endif /* ALTITUDE_H_ */
