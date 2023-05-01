#define BUF_SIZE 10
#define SAMPLE_RATE_HZ 60

//#define TOP_HEIGHT 1225
//#define BOTTOM_HEIGHT 2265
#define Range 1030

//*****************************************************************************
//*****************************************************************************
static circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;    // Counter for the interrupts
static int yaw = 0;

enum DisplayState {PERCENTAGE = 0, MEAN, OFF};

    ADCIntClear(ADC0_BASE, 3);
}

void
YawIntHandler(void)
{
    static int prevState = -1;

    int signalAB = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    switch(prevState)
    {
    case 0:
        signalAB == 1 ? yaw-- : yaw++;
        break;
    case 1:
        signalAB == 3 ? yaw-- : yaw++;
        break;
    case 2:
        signalAB == 0 ? yaw-- : yaw++;
        break;
    case 3:
        signalAB == 2 ? yaw-- : yaw++  ;
        break;
    case -1:
        break;
    };

    prevState = signalAB;

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
    ADCIntEnable(ADC0_BASE, 3);
}

void
initYaw(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);

    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

void
initDisplay (void)
{
}

void
UpdateDisplay(uint32_t meanVal, uint32_t startingVal, uint32_t count, int16_t percentageVal)
UpdateDisplay(uint32_t meanVal, uint32_t startingVal, uint32_t count, int16_t percentageVal, uint32_t yawDeg)
{
    char string[17];  // 16 characters across the display
    char string1[17];

    switch(displayState)
    {
        // Form a new string for the line.  The maximum width specified for the
        //  number field ensures it is displayed right justified.
        usnprintf (string, sizeof(string), "%6d", percentageVal);
        usnprintf (string1, sizeof(string1), "%6d", yawDeg);
        // Update line on display.
        OLEDStringDraw (string, 0, 1);

        usnprintf (string, sizeof(string), "%6d", meanVal);
        OLEDStringDraw (string, 0, 2);

        usnprintf (string, sizeof(string), "%6d", startingVal);
        OLEDStringDraw (string, 0, 3);
        OLEDStringDraw ("Yaw             ", 0, 2);
        OLEDStringDraw (string1, 0, 3);

        break;
    case MEAN:
        usnprintf (string, sizeof(string), "%6d", meanVal);
        OLEDStringDraw (string, 1, 1);

        usnprintf (string, sizeof(string), "%6d", startingVal);
        OLEDStringDraw (string, 0, 2);

        ClearDisplayLine(2);
        ClearDisplayLine(3);

        break;
    int16_t percentageVal;
    uint8_t butState;
    uint32_t startingVal;
    uint32_t display_delay = 100000000 / 16000; // how often display is updated.
    uint32_t display_delay = 100000000 / 64000; // how often display is updated.
    uint32_t display_timer = 0;
    uint32_t yawDeg;


    initClock ();
    initADC ();
    initDisplay ();
    initButtons();
    initYaw();
    initCircBuf (&g_inBuffer, BUF_SIZE);


        //
        // Background task: calculate the (approximate) mean of the values in the
        // circular buffer and display it, together with the sample number.

        sum = 0;
        for (i = 0; i < BUF_SIZE; i++)
            sum = sum + readCircBuf (&g_inBuffer);
        // Calculate and display the rounded mean of the buffer contents
        meanVal = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;

        // int16_t topRange = startingVal - TOP_HEIGHT;
        // int16_t botRange = BOTTOM_HEIGHT - startingVal;

//        if (startingVal >= meanVal)
//        {
//            percentageVal =  (100 * (int16_t)(startingVal - meanVal)) / topRange; // (100 * (startingVal - meanVal)) / startingVal;
//        }
//        else
//        {
//            percentageVal = - (100 * (int16_t)(meanVal - startingVal)) / botRange; // - (int16_t)((100 * (meanVal - startingVal)) / meanVal);
//        }

        if (startingVal >= meanVal)
        {
            percentageVal =  (100 * (int16_t)(startingVal - meanVal)) / Range; // (100 * (startingVal - meanVal)) / startingVal;
            percentageVal = - (100 * (int16_t)(meanVal - startingVal)) / Range; // - (int16_t)((100 * (meanVal - startingVal)) / meanVal);
        }

        yawDeg = (yaw * 360) / 448;

        UpdateInput(&butState, &startingVal, meanVal);

        if(display_timer >= display_delay)
        {
            UpdateDisplay(meanVal, startingVal, g_ulSampCnt, percentageVal);
            UpdateDisplay(meanVal, startingVal, g_ulSampCnt, percentageVal, yawDeg);
            display_timer = 0;
        }
