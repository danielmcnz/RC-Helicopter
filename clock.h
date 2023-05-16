//**********************************************************
// File: clock.h
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// initalizes the clock for the helicopter
//
//**********************************************************

#ifndef CLOCK_H_
#define CLOCK_H_

//**********************************************************
// initClock: initialize the clock for the helicopter
//**********************************************************
inline void initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

#endif /* CLOCK_H_ */
