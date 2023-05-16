//**********************************************************
// File: kernel.c
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// handles all the background tasks using a scheduler
//
//**********************************************************

#include "kernel.h"

#include <stdlib.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/adc.h"

//**********************************************************
// constants
//**********************************************************

#define MAX_TASKS 10

//**********************************************************
// static variables
//**********************************************************

static uint32_t g_ulSampCnt;    // Counter for the interrupts

static uint32_t kernel_frequency;

static task_t* tasks;
static uint8_t n_tasks;

//**********************************************************
// kernelSysTickIntHandler: interrupt handler for enabling ADC trigger 
// and counting interrupts
//**********************************************************
void kernelSysTickIntHandler(void)
{
    ADCProcessorTrigger(ADC0_BASE, 3);
    g_ulSampCnt++;
}

//**********************************************************
// initKernel: initializes the kernel, setting up systick and 
// allocating memory for background tasks
//**********************************************************
void initKernel(uint32_t frequency)
{
    kernel_frequency = frequency;

    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / kernel_frequency);

    // Register the interrupt handler
    SysTickIntRegister(kernelSysTickIntHandler);

    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();

    n_tasks = 0;

    // allocate memory for the kernel tasks
    tasks = malloc(sizeof(task_t) * MAX_TASKS);
}

//**********************************************************
// kernelRegisterTask: creates a new task for the kernel to
// call when required
//**********************************************************
void kernelRegisterTask(uint16_t ticks, void (*run)(void), uint8_t priority)
{
    // assigns a new task to the kernel if there arent already too many tasks
    // currently running in the kernel
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

//**********************************************************
// kernelRun: loops through all the tasks and runs them when
// the task tick reaches zero and resets the tick period
//**********************************************************
void kernelRun(void)
{
    uint8_t task;
    for(task=0;task < n_tasks; ++task)
    {
        // calls the task function to run when the counted ticks reaches zero
        // for that task
         if(tasks[task].cur_tick <= 0)
         {
            tasks[task].run();
            
            // resets the ticks when reaching zero
            tasks[task].cur_tick = tasks[task].ticks;
         }
         else
         {
            // decrements the time till the next call for the specific task
            tasks[task].cur_tick--;
         }
    }
}

//**********************************************************
// compare: helper function for prioritising kernel tasks,
// used for qsort
//**********************************************************
int compare(const void* a, const void* b)
{
    if(((task_t*)a)->priority == ((task_t*)b)->priority) return 0;
    else if (((task_t*)a)->priority < ((task_t*)b)->priority) return -1;
    else return 1;
}

//**********************************************************
// kernelPrioritise: sorts the kernel tasks from highest (0) to lowest (3) priority
//**********************************************************
void kernelPrioritise(void)
{
    qsort(tasks, n_tasks, sizeof(task_t), compare);
}
