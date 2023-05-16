#include <stdint.h>
#include <stdbool.h>
//**********************************************************
// File: control.h

// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Header files for control.c, contains the main
// updateControl() function, and has the PID values set
// as 1000 times their actual value
//
//**********************************************************


//**********************************************************
// PID Values set 1000 times there actual value
//**********************************************************

#define ALTITUDE_KP 650 // 0.650
#define ALTITUDE_KD 800 // 0.800
#define ALTITUDE_KI 12  // 0.012

#define YAW_KP 800      // 0.800
#define YAW_KD 800      // 0.800
#define YAW_KI 9        // 0.009

//**********************************************************
// Checks the current state of the helicoper to overide the
// the desired yaw / altitude to either run the take off / 
// landing sequence
//**********************************************************
void updateControl(void);
