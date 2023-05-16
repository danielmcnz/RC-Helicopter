//**********************************************************
// File: kernel.h
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// handles all the background tasks using a scheduler
//
//**********************************************************

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdint.h>
#include <stdbool.h>

// task_t struct, storing all required info about a task
// for the scheduler to handle and run
typedef struct {
    uint16_t ticks;
    uint8_t priority;
    void (*run)(void);
    int16_t cur_tick;
} task_t;

//**********************************************************
// initKernel: initializes the kernel, setting up systick and 
// allocating memory for background tasks
//**********************************************************
void initKernel(uint32_t sample_rate);

//**********************************************************
// kernelRegisterTask: creates a new task for the kernel to
// call when required
//**********************************************************
void kernelRegisterTask(uint16_t ticks, void (*run)(void), uint8_t priority);

//**********************************************************
// kernelRun: loops through all the tasks and runs them when
// the task tick reaches zero and resets the tick period
//**********************************************************
void kernelRun(void);

//**********************************************************
// kernelPrioritise: sorts the kernel tasks from highest (0) to lowest (3) priority
//**********************************************************
void kernelPrioritise(void);

#endif /* KERNEL_H_ */
