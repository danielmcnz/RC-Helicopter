/*
 * kernel.h
 *
 *  Created on: 7/05/2023
 *      Author: dmc270
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#define MAX_TASKS 10

typedef struct {

} task_t;

void initKernel(uint32_t sample_rate);

void _kernelSysTickIntHandler(void);

#endif /* KERNEL_H_ */
