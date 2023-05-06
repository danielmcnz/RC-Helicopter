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

#define CLK_FREQUENCY 100000000
#define CLK_DELAY_DIVISOR 64000
#define DISPLAY_DELAY CLK_FREQUENCY / CLK_DELAY_DIVISOR // delay between display updates
#define MAX_DISPLAY_LEN 17

enum DisplayState {PERCENTAGE_SCREEN = 0, MEAN_SCREEN, OFF};

void initDisplay(void);

void updateDisplay(uint32_t mean_altitude, uint32_t count, int16_t altitude_perc, YawPosition yaw_pos);

void updateScreenState(void);

void clearDisplayLine(uint8_t line);

bool isLeftButtonPushed(void);

#endif /* DISPLAY_H_ */
