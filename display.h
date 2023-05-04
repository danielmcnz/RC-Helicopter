/*
 * Display.h
 *
 *  Created on: 3/05/2023
 *      Author: dmc270
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdbool.h>
#include <stdint.h>

#include "yaw.h"


#define DISPLAY_DELAY 100000000 / 64000 // MAKE THESE NON MAGIC VARIABLES

enum DisplayState {PERCENTAGE = 0, MEAN, OFF};

static uint8_t displayState = 0;
static bool left_button_pushed = false;

void initDisplay(void);

void updateDisplay(uint32_t mean_altitude, uint32_t count, int16_t altitude_perc, YawPosition yaw_pos);

void _updateScreenState(void);

void clearDisplayLine(uint8_t line);

bool isLeftButtonPushed(void);

#endif /* DISPLAY_H_ */
