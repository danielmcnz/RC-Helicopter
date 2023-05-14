/*
 * uart.c
 *
 *  Created on: 13/05/2023
 *      Author: dmc270
 */

#include <uart.h>

#include "driverlib/sysctl.h"
#include "driverlib/uart.c"
#include "utils/ustdlib.h"

#include "yaw.h"
#include "altitude.h"
#include "heliState.h"
#include "rotors.h"

#define UART_BASE               UART0_BASE
#define DATA_MASK_EIGHT_BITS    UART_CONFIG_WLEN_8
#define STOP_BITS               UART_CONFIG_STOP_ONE
#define PARITY                  UART_CONFIG_PAR_NONE

void initUART(void)
{
    // enable UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // initialize UART, set baud rate, n-bits of data, turn off parity, n stop bits, and stick mode
    UARTConfigSetExpClk(UART_BASE, SysCtlClockGet(), BAUD_RATE, DATA_MASK_EIGHT_BITS | STOP_BITS | PARITY);
}

void sendStatus(void)
{
    char str[MAX_STR_LEN];

    // send desired and actual yaw in deg
    usnprintf(str, sizeof(str), "Yaw: %4d (%4d) deg", getDesiredYaw(), getYaw());
    sendStringUART(str);

    // send desired and actual altitude as a percentage of the max altitude
    usnprintf(str, sizeof(str), "Altitude: %4d (%4d) %", getDesiredAltitude(), getAltitudePerc());
    sendStringUART(str);

    // send duty cycle of main rotor
    usnprintf(str, sizeof(str), "Main Rotor %: %4d%", getMainRotorDutyCycle());
    sendStringUART(str);

    // send duty cycle of secondary rotor
    usnprintf(str, sizeof(str), "Sec Rotor %: %4d%", getSecondaryRotorDutyCycle());
    sendStringUART(str);

    // send current helicopter operating mode
    usnprintf(str, sizeof(str), "heli state: %8d", getHeliStateStr());
    sendStringUART(str);
}

void sendStringUART(char *str)
{
    while(*str != '\0')
    {
        UARTCharPut(UART_BASE, *str);

        ++str;
    }
}
