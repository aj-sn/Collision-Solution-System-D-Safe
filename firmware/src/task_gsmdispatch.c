/*******************************************************************************
  MPLAB Harmony Application Source File

  File Name:
    task_gsmdispatch.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "task_gsmdispatch.h"
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "CSSDefinitions.h"             // System specific defines
#include <stdint.h>
#include <string.h>

extern collisionFlags collision;

extern uint16_t secondsCounter;
uint16_t secondsCount;

extern const uint8_t testGSM[];
extern uint8_t replyGSM[];
extern uint8_t tempReplyGSM[];
extern const uint8_t gsmTextSetup1[];
extern const uint8_t gsmTextSetup2[];
extern const uint8_t gsmTextSetup3[];
extern const uint8_t gsmContactInfo[];
extern const uint8_t gsmMessage1[];
extern const uint8_t gsmMessage2[];
extern const uint8_t gsmMessage3[];
extern const uint8_t gsmSendCMD[];
extern const uint8_t gsmCallSetup[];
extern const uint8_t gsmCall[];

extern volatile bool txThresholdEventReceivedGSM;
extern uint16_t rxCounterGSM;
extern volatile uint32_t nBytesReadGSM;   //for GSM data

extern char location[50];
extern char gmLink[70];

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the TASK_GSMDISPATCH_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

TASK_GSMDISPATCH_DATA task_gsmdispatchData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SERCOM1_USART_Pusher(uint8_t *)

  Remarks:
    To call multiple USART_Writes
 */

void SERCOM1_USART_Pusher(uint8_t *message)
{
    SERCOM1_USART_Write((uint8_t*)message, strlen((char*)message));
    while(txThresholdEventReceivedGSM == false)
    {
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    txThresholdEventReceivedGSM = false;
    vTaskDelay(5000 / portTICK_PERIOD_MS);
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void TASK_GSMDISPATCH_Initialize ( void )

  Remarks:
    See prototype in task_gsmdispatch.h.
 */

void TASK_GSMDISPATCH_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    task_gsmdispatchData.state = TASK_GSMDISPATCH_STATE_INIT;
}


/******************************************************************************
  Function:
    void TASK_GSMDISPATCH_Tasks ( void )

  Remarks:
    See prototype in task_gsmdispatch.h.
 */

void TASK_GSMDISPATCH_Tasks ( void )
{   
    while(true)
    {   
        if(collision.accelerometer == 1 && collision.rangeFinder == 1)
        {
            Panic_Output_Set();
            NVIC_INT_Disable();
            secondsCounter = 0;
            NVIC_INT_Enable();
            while(secondsCount <= PANIC_WAIT_THRESHOLD)
            {
                vTaskDelay(1 / portTICK_PERIOD_MS);
                NVIC_INT_Disable();
                secondsCount = secondsCounter;
                NVIC_INT_Enable();
                if(Unpanic_Input_Get() == 0)
                {
                    break;
                }
            }
            if(secondsCount > PANIC_WAIT_THRESHOLD)
            {
                collision.PanicState = 1;
            }
            else
            {
                collision.PanicState = 0;
                collision.accelerometer = 0;
                collision.rangeFinder = 0;
                Panic_Output_Clear();
                vTaskDelay(20 / portTICK_PERIOD_MS);
            }
        }

        if(collision.accelerometer == 1 && collision.rangeFinder == 1 && collision.PanicState == 1)
        {
            SERCOM1_USART_Pusher((uint8_t*)testGSM);
            SERCOM1_USART_Pusher((uint8_t*)gsmTextSetup1);
            SERCOM1_USART_Pusher((uint8_t*)gsmTextSetup2);
            SERCOM1_USART_Pusher((uint8_t*)gsmTextSetup3);
            SERCOM1_USART_Pusher((uint8_t*)gsmContactInfo);
            SERCOM1_USART_Pusher((uint8_t*)gsmMessage1);
            SERCOM1_USART_Pusher((uint8_t*)gsmSendCMD);
            SERCOM1_USART_Pusher((uint8_t*)gsmContactInfo);
            SERCOM1_USART_Pusher((uint8_t*)gsmMessage2);
            SERCOM1_USART_Pusher((uint8_t*)gsmSendCMD);
            
            if(collision.gpsReady == 1)
            {
                SERCOM1_USART_Pusher((uint8_t*)gsmContactInfo);
                SERCOM1_USART_Pusher((uint8_t*)location);
                SERCOM1_USART_Pusher((uint8_t*)gsmSendCMD);
                SERCOM1_USART_Pusher((uint8_t*)gsmContactInfo);
                SERCOM1_USART_Pusher((uint8_t*)gmLink);
                SERCOM1_USART_Pusher((uint8_t*)gsmSendCMD);    
            }
            else if((collision.gpsReady == 0) && (location[40] != '\0') && (gmLink[65] != '\0'))
            {
                SERCOM1_USART_Pusher((uint8_t*)gsmContactInfo);
                SERCOM1_USART_Pusher((uint8_t*)gsmMessage3);
                SERCOM1_USART_Pusher((uint8_t*)gsmSendCMD);
                SERCOM1_USART_Pusher((uint8_t*)gsmContactInfo);
                SERCOM1_USART_Pusher((uint8_t*)location);
                SERCOM1_USART_Pusher((uint8_t*)gsmSendCMD);
                SERCOM1_USART_Pusher((uint8_t*)gsmContactInfo);
                SERCOM1_USART_Pusher((uint8_t*)gmLink);
                SERCOM1_USART_Pusher((uint8_t*)gsmSendCMD);
            }
            //printf("\n message sent");
            Panic_Output_Clear();
            vTaskDelay(100000000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }        
}

/*******************************************************************************
 End of File
 */
