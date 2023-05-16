//**********************************************************
// File: display.c
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Handles the OLED display on the tiva board and displays
// altitude and yaw information
//
//**********************************************************

#include "display.h"

#include "buttons4.h"
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

#include "altitude.h"
#include "yaw.h"

//**********************************************************
// constants
//**********************************************************

#define MAX_DISPLAY_LEN 17

//**********************************************************
// static variables
//**********************************************************

static uint8_t display_cycle = 0;

//**********************************************************
// initDisplay: intialise the Orbit OLED display
//**********************************************************
void initDisplay(void)
{
    OLEDInitialise ();
}

//**********************************************************
// updateDisplay: update the display of the OLED,
// displaying chunks at a time to ensure it doesn't
// take up too much time of each clock cycle
//**********************************************************
void updateDisplay()
{
    char altitude_perc_str[MAX_DISPLAY_LEN];  // 16 characters across the display
    char yaw_degree_str[MAX_DISPLAY_LEN];
    char yaw_sub_degree_str[MAX_DISPLAY_LEN];

    // displays part of the status each clock cycle, constantly looping through each case
    switch(display_cycle)
    {
    case 0:
        OLEDStringDraw ("Percentage ADC  ", 0, 0);
        break;
    case 1:
        // displays altitude as a percentage between 0 to 100
        usnprintf (altitude_perc_str, sizeof(altitude_perc_str), "%3d", getAltitudePerc());
        OLEDStringDraw (altitude_perc_str, 0, 1);
        OLEDStringDraw ("%%", 6, 1);
        break;
    case 2:
        // displays degree precision yaw
        usnprintf (yaw_degree_str, sizeof(yaw_degree_str), "%4d", getYaw().degree);
        OLEDStringDraw ("Yaw (Deg)       ", 0, 2);
        OLEDStringDraw (yaw_degree_str, 0, 3);
        break;
    case 3:
        // displays sub-degree precision yaw
        usnprintf (yaw_sub_degree_str, sizeof(yaw_sub_degree_str), "%1d", getYaw().sub_degree);
        OLEDStringDraw(".", 4, 3);
        OLEDStringDraw (yaw_sub_degree_str, 5, 3);
        break;
    }

    ++display_cycle;

    if(display_cycle > 3)
        display_cycle = 0;
}

//**********************************************************
// clearDisplayLine: clears the given row in the OLED display
//**********************************************************
void clearDisplayLine(uint8_t line)
{
    OLEDStringDraw ("                ", 0, line);
}
