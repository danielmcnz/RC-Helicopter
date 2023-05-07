/*
 * clock.h
 *
 *  Created on: 7/05/2023
 *      Author: dmc270
 */

#ifndef CLOCK_H_
#define CLOCK_H_


inline void initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

#endif /* CLOCK_H_ */
