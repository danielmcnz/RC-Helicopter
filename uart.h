//**********************************************************
// File: uart.h
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Handles UART output and displays a status of the helicopters
// position and state
//
//**********************************************************

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>

//**********************************************************
// initUART: initialize UART (1 stop bit, no parity, 8-bit data, 9600 baud rate)
//**********************************************************
void initUART(void);

//**********************************************************
// sendStatus: displays the status of the helicopter position
// and state through uart
//**********************************************************
void sendStatus(void);

//**********************************************************
// sendStringUART: helper function to print a string through uart
//**********************************************************
void sendStringUART(char *str);

#endif /* UART_H_ */
