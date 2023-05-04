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

static uint8_t displayState = 0;
static bool left_button_pushed = false;
static uint8_t button_state;

void initDisplay(void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}

void updateDisplay(uint32_t mean_altitude, uint32_t count, int16_t altitude_perc, YawPosition yaw_pos)
{
    char string[MAX_DISPLAY_LEN];  // 16 characters across the display
    char string1[MAX_DISPLAY_LEN];

    switch(displayState)
    {
    case PERCENTAGE_SCREEN:
        OLEDStringDraw ("Percentage ADC  ", 0, 0);

        // Form a new string for the line.  The maximum width specified for the
        //  number field ensures it is displayed right justified.
        usnprintf (string, sizeof(string), "%6d", altitude_perc);
        usnprintf (string1, sizeof(string1), "%6d", yaw_pos.degree);
        // Update line on display.
        OLEDStringDraw (string, 0, 1);

        OLEDStringDraw ("Yaw (Deg)       ", 0, 2);
        OLEDStringDraw (string1, 0, 3);

        break;
    case MEAN_SCREEN:
        OLEDStringDraw ("Mean ADC        ", 0, 0);

        usnprintf (string, sizeof(string), "%6d", mean_altitude);
        OLEDStringDraw (string, 1, 1);

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

void _updateScreenState(void)
{

    left_button_pushed = false;

    updateButtons ();

    button_state = checkButton (LEFT);
    if(button_state == PUSHED)
        left_button_pushed = true;

    button_state = checkButton(UP);
    if(button_state == PUSHED)
    {
        displayState++;
        if(displayState > OFF)
            displayState = 0;
    }
}

void clearDisplayLine(uint8_t line)
{
    OLEDStringDraw ("                ", 0, line);
}

bool isLeftButtonPushed(void)
{
    return left_button_pushed;
};
