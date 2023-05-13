/*
 * kernel.c
 *
 *  Created on: 7/05/2023
 *      Author: dmc270
 */

#include <kernel.h>

#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

static uint32_t g_ulSampCnt;    // Counter for the interrupts

static task_t* tasks;

void initKernel(uint32_t sample_rate)
{
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / sample_rate);

    // Register the interrupt handler
    SysTickIntRegister(_kernelSysTickIntHandler);

    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

void _kernelSysTickIntHandler(void)
{
    g_ulSampCnt++;
}
