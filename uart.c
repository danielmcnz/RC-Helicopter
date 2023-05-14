/*
 * uart.c
 *
 *  Created on: 13/05/2023
 *      Author: dmc270
 */

#include "uart.h"

#include "driverlib/sysctl.h"
#include "driverlib/uart.c"
#include "utils/ustdlib.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // initialize UART, set baud rate, n-bits of data, turn off parity, n stop bits, and stick mode
    UARTConfigSetExpClk(UART_BASE, SysCtlClockGet(), BAUD_RATE, DATA_MASK_EIGHT_BITS | STOP_BITS | PARITY);

    UARTFIFOEnable(UART_BASE);
    UARTEnable(UART_BASE);
}

void sendStatus(void)
{
    char str[MAX_STR_LEN];

    sendStringUART("\r\n------------\r\n");

    // send desired and actual yaw in deg
    usnprintf(str, sizeof(str), "Yaw: %4d (%4d) deg \r\n", getDesiredYaw(), getYaw());
    sendStringUART(str);

    // send desired and actual altitude as a percentage of the max altitude
    usnprintf(str, sizeof(str), "Altitude: %4d (%4d) % \r\n", getDesiredAltitude(), getAltitudePerc());
    sendStringUART(str);

    // send duty cycle of main rotor
    usnprintf(str, sizeof(str), "Main Rotor %: %4d% \r\n", getMainRotorDutyCycle());
    sendStringUART(str);

    // send duty cycle of secondary rotor
    usnprintf(str, sizeof(str), "Sec Rotor %: %4d% \r\n", getSecondaryRotorDutyCycle());
    sendStringUART(str);

    // send current helicopter operating mode
    usnprintf(str, sizeof(str), "state: %10d \r\n", getHeliStateStr());
    sendStringUART(str);

    sendStringUART("------------\r\n");
}

void sendStringUART(char *str)
{
    while(*str != '\0')
    {
        UARTCharPut(UART_BASE, *str);

        ++str;
    }
}
