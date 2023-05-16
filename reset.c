//**********************************************************
// File: reset.c
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Enables a soft reset for the helicopter through ISR
//
//**********************************************************

#include "reset.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

//**********************************************************
// resetIntHandler: interrupt handler that resets the program through sysctl
//**********************************************************
void resetIntHandler(void)
{
    SysCtlReset();
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_6);
}

//**********************************************************
// initReset: enables the reset pin and registers the reset interrupt handler
//**********************************************************
void initReset()
{
    // enable the reset pin port
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // configures reset pin to be used as a GPIO input
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_6);
    
    // sets the interrupt type for the reset as falling edge triggered
    GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_FALLING_EDGE);

    // registers the reset interrupt handler
    GPIOIntRegister(GPIO_PORTA_BASE, resetIntHandler);

    // enable the gpio interrupt for reset
    GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_6);
}
