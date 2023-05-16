//**********************************************************
// File: display.h
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Handles the OLED display on the tiva board and displays
// altitude and yaw information
//
//**********************************************************

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdbool.h>
#include <stdint.h>

//**********************************************************
// initDisplay: intialise the Orbit OLED display
//**********************************************************
void initDisplay(void);

//**********************************************************
// updateDisplay: update the display of the OLED,
// displaying chunks at a time to ensure it doesn't
// take up too much time of each clock cycle
//**********************************************************
void updateDisplay();

//**********************************************************
// clearDisplayLine: clears the given row in the OLED display
//**********************************************************
void clearDisplayLine(uint8_t line);

#endif /* DISPLAY_H_ */
