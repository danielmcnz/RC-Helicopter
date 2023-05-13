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
//static bool left_button_pushed = false;
//static uint8_t button_state;

void initDisplay(void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}

void updateDisplay(uint32_t mean_altitude, uint32_t count, int16_t altitude_perc, YawPosition yaw_pos)
{
    char altitude_perc_str[MAX_DISPLAY_LEN];  // 16 characters across the display
    char yaw_degree_str[MAX_DISPLAY_LEN];
    char yaw_sub_degree_str[MAX_DISPLAY_LEN];

    switch(displayState)
    {
    case PERCENTAGE_SCREEN:
        OLEDStringDraw ("Percentage ADC  ", 0, 0);

        // Form a new string for the line.  The maximum width specified for the
        //  number field ensures it is displayed right justified.
        usnprintf (altitude_perc_str, sizeof(altitude_perc_str), "%6d", altitude_perc);
        usnprintf (yaw_degree_str, sizeof(yaw_degree_str), "%4d", yaw_pos.degree);
        usnprintf (yaw_sub_degree_str, sizeof(yaw_sub_degree_str), "%1d", yaw_pos.sub_degree);
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

        usnprintf (altitude_perc_str, sizeof(altitude_perc_str), "%6d", mean_altitude);
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

//void updateScreenState(void)
//{
//
//    left_button_pushed = false;
//
//    updateButtons ();
//
//    button_state = checkButton (LEFT);
//    if(button_state == PUSHED)
//        left_button_pushed = true;
//
//    button_state = checkButton(UP);
//    if(button_state == PUSHED)
//    {
//        displayState++;
//        if(displayState > OFF)
//            displayState = 0;
//    }
//}

void clearDisplayLine(uint8_t line)
{
    OLEDStringDraw ("                ", 0, line);
}

//bool isLeftButtonPushed(void)
//{
//    return left_button_pushed;
//}
