//**********************************************************
// File: reset.h
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Enables a soft reset for the helicopter through ISR
//
//**********************************************************

#ifndef RESET_H_
#define RESET_H_

#include <stdint.h>
#include <stdbool.h>

//**********************************************************
// initReset: enables the reset pin and registers the reset interrupt handler
//**********************************************************
void initReset();

#endif /* RESET_H_ */
