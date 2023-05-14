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

static uint8_t display_state = 0;

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

    switch(display_state)
    {
    case PERCENTAGE_SCREEN:
        OLEDStringDraw ("Percentage ADC  ", 0, 0);

        // Form a new string for the line.  The maximum width specified for the
        //  number field ensures it is displayed right justified.
        usnprintf (altitude_perc_str, sizeof(altitude_perc_str), "%6d", /*getAltitudePerc()*/ getAltitudeError());
        usnprintf (yaw_degree_str, sizeof(yaw_degree_str), "%4d", getYaw().degree);
        usnprintf (yaw_sub_degree_str, sizeof(yaw_sub_degree_str), "%1d", getYaw().sub_degree);
        // Update line on display.
        OLEDStringDraw (altitude_perc_str, 0, 1);
        OLEDStringDraw ("%", 6, 1);

        OLEDStringDraw ("Yaw (Deg)       ", 0, 2);
        OLEDStringDraw (yaw_degree_str, 0, 3);
        OLEDStringDraw(".", 4, 3);
        OLEDStringDraw (yaw_sub_degree_str, 5, 3);

        break;
    case MEAN_SCREEN:
        OLEDStringDraw ("Mean ADC        ", 0, 0);

        usnprintf (altitude_perc_str, sizeof(altitude_perc_str), "%6d", getMeanAltitude());
        OLEDStringDraw (altitude_perc_str, 1, 1);

        clearDisplayLine(2);
        clearDisplayLine(3);

        break;
    case OFF:
        clearDisplayLine(0);
        clearDisplayLine(1);
        clearDisplayLine(2);
        clearDisplayLine(3);
        break;
    }
}

void clearDisplayLine(uint8_t line)
{
    OLEDStringDraw ("                ", 0, line);
}

void incDisplayState(void)
{
    display_state++;
    if(display_state > OFF)
        display_state = 0;
}
