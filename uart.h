/*
 * uart.h
 *
 *  Created on: 13/05/2023
 *      Author: dmc270
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_STR_LEN 19

#define BAUD_RATE   9600

void initUART(void);

void sendStatus(void);

void sendStringUART(char *str);

#endif /* UART_H_ */
