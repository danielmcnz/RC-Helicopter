//**********************************************************
// File: uart.c
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Handles UART output and displays a status of the helicopters
// position and state
//
//**********************************************************

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

//**********************************************************
// constants
//**********************************************************

#define UART_BASE               UART0_BASE
#define DATA_MASK_EIGHT_BITS    UART_CONFIG_WLEN_8
#define STOP_BITS               UART_CONFIG_STOP_ONE
#define PARITY                  UART_CONFIG_PAR_NONE

// max line length when displaying uart output
#define MAX_STR_LEN 28

// baud rate for the uart console output
#define BAUD_RATE   9600

//**********************************************************
// initUART: initialize UART (1 stop bit, no parity, 8-bit data, 9600 baud rate)
//**********************************************************
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

//**********************************************************
// sendStatus: displays the status of the helicopter position
// and state through uart
//**********************************************************
void sendStatus(void)
{
    char str[MAX_STR_LEN];

    // send desired and actual yaw in deg
    usnprintf(str, sizeof(str), "Yaw: %3d (%1d) \r\n\0", getDesiredYaw(), getYaw().degree);
    sendStringUART(str);

    // send desired and actual altitude as a percentage of the max altitude
    usnprintf(str, sizeof(str), "Alt: %3d (%3d) \r\n\0" , getDesiredAltitude(), getAltitudePerc());
    sendStringUART(str);

    // send duty cycle of main rotor
    usnprintf(str, sizeof(str), "Main %3d Tail %3d \r\n\0", getMainRotorDutyCycle(), getSecondaryRotorDutyCycle());
    sendStringUART(str);

    // send current helicopter operating mode
    usnprintf(str, sizeof(str), "state: %11s \r\n\0", getHeliStateStr());
    sendStringUART(str);

    sendStringUART("------------\r\n\0");
}

//**********************************************************
// sendStringUART: helper function to print a string through uart
//**********************************************************
void sendStringUART(char *str)
{
    // loop through the given string to retreive each character
    while(*str != '\0')
    {
        // send each char through UART
        UARTCharPut(UART_BASE, *str);

        ++str;
    }
}
