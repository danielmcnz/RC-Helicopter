/*
 * kernel.h
 *
 *  Created on: 7/05/2023
 *      Author: dmc270
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_TASKS 10

typedef struct {
    uint16_t ticks;
    uint8_t priority;
    void (*run)(void);
    int16_t cur_tick;
} task_t;

void initKernel(uint32_t sample_rate);

void kernelSysTickIntHandler(void);

void kernelRegisterTask(uint16_t ticks, void (*run)(void), uint8_t priority);

void kernelRun(void);

void kernelPrioritise(void);

#endif /* KERNEL_H_ */
