/*******************************************************************************
  MPLAB Harmony Application Source File

  File Name:
    task_rangefinder.c

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

#include "task_rangefinder.h"
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "CSSDefinitions.h"             // System specific defines
#include <stdint.h>

extern collisionFlags collision;

extern volatile bool tc3_EIC9echoA_buffer_ready;
extern volatile bool tc4_EIC7echoB_buffer_ready;
extern volatile bool tc5_EIC5echoC_buffer_ready;
extern volatile bool tc6_EIC3echoD_buffer_ready;
extern uint16_t pulseWidthA;
extern uint16_t pulseWidthB;
extern uint16_t pulseWidthC;
extern uint16_t pulseWidthD;
uint16_t distanceA;
uint16_t distanceB;
uint16_t distanceC;
uint16_t distanceD;
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
    This structure should be initialized by the TASK_RANGEFINDER_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

TASK_RANGEFINDER_DATA task_rangefinderData;

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


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void TASK_RANGEFINDER_Initialize ( void )

  Remarks:
    See prototype in task_rangefinder.h.
 */

void TASK_RANGEFINDER_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    task_rangefinderData.state = TASK_RANGEFINDER_STATE_INIT;
}


/******************************************************************************
  Function:
    void TASK_RANGEFINDER_Tasks ( void )

  Remarks:
    See prototype in task_rangefinder.h.
 */

void TASK_RANGEFINDER_Tasks ( void )
{

    while(true)
    {
        TrigPinA_Clear();
        TrigPinB_Clear();
        TrigPinC_Clear();
        TrigPinD_Clear();
        TrigPinA_Set();
        TrigPinB_Set();
        TrigPinC_Set();
        TrigPinD_Set();
        vTaskDelay(2 / portTICK_PERIOD_MS);
        TrigPinA_Clear();
        TrigPinB_Clear();
        TrigPinC_Clear();
        TrigPinD_Clear();
        
        if(tc3_EIC9echoA_buffer_ready == true)
        {
            tc3_EIC9echoA_buffer_ready = false;
            pulseWidthA = TC3_Capture16bitChannel1Get();
            distanceA = (pulseWidthA * 0.034)/2;
            //printf("Range of A is %d\n", distanceA);
            if(distanceA <= 200 && distanceA >= 100)
            {
                collision.rangeFinder = 1;
            }
        }
        
        if(tc4_EIC7echoB_buffer_ready == true)
        {
            tc4_EIC7echoB_buffer_ready = false;
            pulseWidthB = TC4_Capture16bitChannel1Get();
            distanceB = (pulseWidthB * 0.034)/2;
            //printf("Range of B is %d\n", distanceB);
            if(distanceB <= 200 && distanceB >= 100)
            {
                collision.rangeFinder = 1;
            }
        }
        
        if(tc5_EIC5echoC_buffer_ready == true)
        {
            tc5_EIC5echoC_buffer_ready = false;
            pulseWidthC = TC5_Capture16bitChannel1Get();
            distanceC = (pulseWidthC * 0.034)/2;
            //printf("Range of C is %d\n", distanceC);
            if(distanceC <= 200 && distanceC >= 100)
            {
                collision.rangeFinder = 1;
            }
        }
        
        if(tc6_EIC3echoD_buffer_ready == true)
        {
            tc6_EIC3echoD_buffer_ready = false;
            pulseWidthD = TC6_Capture16bitChannel1Get();
            distanceD = (pulseWidthD * 0.034)/2;
            //printf("Range of D is %d\n", distanceD);
            if(distanceD <= 200 && distanceD >= 100)
            {
                collision.rangeFinder = 1;
            }
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);  
    }
    
}


/*******************************************************************************
 End of File
 */
