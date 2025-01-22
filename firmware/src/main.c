/*******************************************************************************
  Main Source File

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "CSSDefinitions.h"             // System specific defines
#include <stdint.h>
#include <string.h>

collisionFlags collision;

uint16_t secondsCounter;

uint8_t gpsString[256] = {};
uint8_t tempGpsString[256] = {};
uint16_t rxCounterGPS; //for GPS data
volatile uint32_t nBytesReadGPS;   //for GPS data

const uint8_t testGSM[] = "\rATE0\r\n";
uint8_t replyGSM[] = "";
uint8_t tempReplyGSM[] = "";
const uint8_t gsmTextSetup1[] = "\rAT+CMGF=1\r\n";
const uint8_t gsmTextSetup2[] = "\rAT+CSCS=\"GSM\"\r\n";
const uint8_t gsmTextSetup3[] = "\rAT+CSMP=17,167,0,0\r\n";
const uint8_t gsmContactInfo[] = "\rAT+CMGS=\"0000000000\"\r\n";
const uint8_t gsmMessage1[] = "This is the D-Safe system in Mr.KingPins car\nHe has been in an accident, please call 911";
const uint8_t gsmMessage2[] = "Sending his current GPS location and probable UTC time of the event";
const uint8_t gsmMessage3[] = "Current GPS data not available, sending last known GPS location and time";
const uint8_t gsmSendCMD[] = "\x1a";
const uint8_t gsmCallSetup[] = "\rAT+CSDVC=1\r\n";
const uint8_t gsmCall[] = "\rATD0000000000;\r\n";
volatile bool txThresholdEventReceivedGSM = false;
uint16_t rxCounterGSM;
volatile uint32_t nBytesReadGSM;   //for GPS data

uint8_t accelString[256] = {};
uint8_t tempAccelString[256] = {};
uint16_t rxCounterAccel;
volatile uint32_t nBytesReadAccel;

volatile bool tc3_EIC9echoA_buffer_ready = false;
volatile bool tc4_EIC7echoB_buffer_ready = false;
volatile bool tc5_EIC5echoC_buffer_ready = false;
volatile bool tc6_EIC3echoD_buffer_ready = false;
uint16_t pulseWidthA;
uint16_t pulseWidthB;
uint16_t pulseWidthC;
uint16_t pulseWidthD;
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

/*The interrupt handle functions for timers*/
/*This function is called after TC3 period expires*/
void TC7_Callback_InterruptHandler(TC_TIMER_STATUS status, uintptr_t context)
{
    secondsCounter++;
}

//GPS interrupt callback function
void usartReadEventHandlerGPS(SERCOM_USART_EVENT event, uintptr_t context )
{
    uint32_t nBytesAvailable = 0;
    
    if (event == SERCOM_USART_EVENT_READ_THRESHOLD_REACHED)
    {
        /* Receiver should atleast have the thershold number of bytes in the ring buffer */
        nBytesAvailable = SERCOM0_USART_ReadCountGet();
        
        nBytesReadGPS += SERCOM0_USART_Read((uint8_t*)&gpsString[nBytesReadGPS], nBytesAvailable);
    }
    
    if(nBytesReadGPS > 255)
    {
        nBytesReadGPS = 0;
        rxCounterGPS = 0;
        memset(gpsString, '\0', strlen((const char*)gpsString));
    }
}

//GSM interrupt callback functions
void usartReadEventHandlerGSM(SERCOM_USART_EVENT event, uintptr_t context )
{
    uint32_t nBytesAvailable = 0;
    
    if (event == SERCOM_USART_EVENT_READ_THRESHOLD_REACHED)
    {
        /* Receiver should atleast have the thershold number of bytes in the ring buffer */
        nBytesAvailable = SERCOM1_USART_ReadCountGet();
        
        nBytesReadGSM += SERCOM1_USART_Read((uint8_t*)&replyGSM[nBytesReadGSM], nBytesAvailable);                          
    }
    if(nBytesReadGSM > 49)
    {
        nBytesReadGSM = 0;
        rxCounterGSM = 0;
        memset(replyGSM, '\0', strlen((const char*)replyGSM));
    }
}

void usartWriteEventHandlerGSM(SERCOM_USART_EVENT event, uintptr_t context )
{
    txThresholdEventReceivedGSM = true;
}



//Accelerometer interrupt callback function
void usartReadEventHandlerAccel(SERCOM_USART_EVENT event, uintptr_t context )
{
    uint32_t nBytesAvailable = 0;
    
    if (event == SERCOM_USART_EVENT_READ_THRESHOLD_REACHED)
    {
        /* Receiver should atleast have the thershold number of bytes in the ring buffer */
        nBytesAvailable = SERCOM4_USART_ReadCountGet();
        
        nBytesReadAccel += SERCOM4_USART_Read((uint8_t*)&accelString[nBytesReadAccel], nBytesAvailable);
    }
    
    if(nBytesReadAccel > 220)
    {
        nBytesReadAccel = 0;
        rxCounterAccel = 0;
        memset(accelString, '\0', strlen((const char*)accelString));
    }
}





void EIC9echoA_handler( TC_CAPTURE_STATUS status, uintptr_t context)
{
    if ((status  & TC_CAPTURE_STATUS_CAPTURE0_READY) == TC_CAPTURE_STATUS_CAPTURE0_READY)
    {
        tc3_EIC9echoA_buffer_ready = true;
    }
}

void EIC7echoB_handler( TC_CAPTURE_STATUS status, uintptr_t context)
{
    if ((status  & TC_CAPTURE_STATUS_CAPTURE0_READY) == TC_CAPTURE_STATUS_CAPTURE0_READY)
    {
        tc4_EIC7echoB_buffer_ready = true;
    }
}

void EIC5echoC_handler( TC_CAPTURE_STATUS status, uintptr_t context)
{
    if ((status  & TC_CAPTURE_STATUS_CAPTURE0_READY) == TC_CAPTURE_STATUS_CAPTURE0_READY)
    {
        tc5_EIC5echoC_buffer_ready = true;
    }
}

void EIC3echoD_handler( TC_CAPTURE_STATUS status, uintptr_t context)
{
    if ((status  & TC_CAPTURE_STATUS_CAPTURE0_READY) == TC_CAPTURE_STATUS_CAPTURE0_READY)
    {
        tc6_EIC3echoD_buffer_ready = true;
    }
}


int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    
    /*The call-back function implementation for timers*/
    TC7_TimerCallbackRegister(TC7_Callback_InterruptHandler, (uintptr_t)NULL);
    /*Timer start*/
    TC7_TimerStart();
    
    TC3_CaptureCallbackRegister(EIC9echoA_handler, (uintptr_t)NULL);
    TC4_CaptureCallbackRegister(EIC7echoB_handler, (uintptr_t)NULL);
    TC5_CaptureCallbackRegister(EIC5echoC_handler, (uintptr_t)NULL);
    TC6_CaptureCallbackRegister(EIC3echoD_handler, (uintptr_t)NULL);
    
    TC3_CaptureStart();
    TC4_CaptureStart();
    TC5_CaptureStart();
    TC6_CaptureStart();
    
    SERCOM0_USART_ReadCallbackRegister(usartReadEventHandlerGPS, (uintptr_t) NULL);
    /*Set a threshold value to receive a callback after every 20 characters are received */
    SERCOM0_USART_ReadThresholdSet(20);
    /* Enable RX event notifications */
    SERCOM0_USART_ReadNotificationEnable(true, false);
    
    /* Register a callback for write events */
    SERCOM1_USART_WriteCallbackRegister(usartWriteEventHandlerGSM, (uintptr_t) NULL);
    /* Register a callback for read events */
    SERCOM1_USART_ReadCallbackRegister(usartReadEventHandlerGSM, (uintptr_t) NULL); 
    /* Let's enable notifications to get notified when the TX buffer is empty */
    SERCOM1_USART_WriteThresholdSet(SERCOM1_USART_WriteBufferSizeGet());   
    /* Enable notifications */
    SERCOM1_USART_WriteNotificationEnable(true, false);
    /*Set a threshold value to receive a callback after every 2 characters are received */
    SERCOM1_USART_ReadThresholdSet(2);
    /* Enable RX event notifications */
    SERCOM1_USART_ReadNotificationEnable(true, false);
    
    SERCOM4_USART_ReadCallbackRegister(usartReadEventHandlerAccel, (uintptr_t) NULL);
    /*Set a threshold value to receive a callback after every 20 characters are received */
    SERCOM4_USART_ReadThresholdSet(20);
    /* Enable RX event notifications */
    SERCOM4_USART_ReadNotificationEnable(true, false);
 
    while ( true )
    {   
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );   
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

