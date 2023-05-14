/*
 * kernel.c
 *
 *  Created on: 7/05/2023
 *      Author: dmc270
 */

#include <kernel.h>

#include <stdlib.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/adc.h"

static uint32_t g_ulSampCnt;    // Counter for the interrupts

static uint32_t kernel_frequency;

static task_t* tasks;
static uint8_t n_tasks;

void initKernel(uint32_t frequency)
{
    kernel_frequency = frequency;

    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / kernel_frequency);

    // Register the interrupt handler
    SysTickIntRegister(_kernelSysTickIntHandler);

    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();

    n_tasks = 0;

    tasks = malloc(sizeof(task_t) * MAX_TASKS);
}

void _kernelSysTickIntHandler(void)
{
    ADCProcessorTrigger(ADC0_BASE, 3);
    g_ulSampCnt++;
}

void kernelRegisterTask(uint16_t ticks, void (*run)(void), uint8_t priority)
{
    if(n_tasks < MAX_TASKS)
    {
        task_t task;
        task.ticks = ticks;
        task.run = run;
        task.priority = priority;
        task.cur_tick = 0;

        tasks[n_tasks] = task;

        ++n_tasks;
    }
}

void kernelRun(void)
{
    uint8_t task;
    for(task=0;task <= n_tasks; ++task)
    {
         if(tasks[task].cur_tick == 0)
         {
            tasks[task].run();

             tasks[task].cur_tick = tasks[task].ticks;
         }
         else
         {
             tasks[task].cur_tick--;
         }
    }
}

int compare(const void* a, const void* b)
{
    if(((task_t*)a)->priority == ((task_t*)b)->priority) return 0;
    else if (((task_t*)a)->priority < ((task_t*)b)->priority) return -1;
    else return 1;
}

void kernelPrioritise(void)
{
    qsort(tasks, n_tasks, sizeof(task_t), compare);
}
