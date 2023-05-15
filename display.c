/*
 * Display.cpp
 *
 *  Created on: 3/05/2023
 *      Author: dmc270
 */

#include "display.h"

#include "buttons4.h"
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

#include "altitude.h"
#include "yaw.h"

static uint8_t display_cycle = 0;

void initDisplay(void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}

void updateDisplay()
{
    char altitude_perc_str[MAX_DISPLAY_LEN];  // 16 characters across the display
    char yaw_degree_str[MAX_DISPLAY_LEN];
    char yaw_sub_degree_str[MAX_DISPLAY_LEN];

    switch(display_cycle)
    {
    case 0:
        OLEDStringDraw ("Percentage ADC  ", 0, 0);
        break;
    case 1:
        usnprintf (altitude_perc_str, sizeof(altitude_perc_str), "%3d", getAltitudePerc());
        OLEDStringDraw (altitude_perc_str, 0, 1);
        OLEDStringDraw ("%%", 6, 1);
        break;
    case 2:
        usnprintf (yaw_degree_str, sizeof(yaw_degree_str), "%4d", getYaw().degree);
        OLEDStringDraw ("Yaw (Deg)       ", 0, 2);
        OLEDStringDraw (yaw_degree_str, 0, 3);
        break;
    case 3:
        usnprintf (yaw_sub_degree_str, sizeof(yaw_sub_degree_str), "%1d", getYaw().sub_degree);
        OLEDStringDraw(".", 4, 3);
        OLEDStringDraw (yaw_sub_degree_str, 5, 3);
        break;
    }

    ++display_cycle;

    if(display_cycle > 3)
        display_cycle = 0;
}

void clearDisplayLine(uint8_t line)
{
    OLEDStringDraw ("                ", 0, line);
}
