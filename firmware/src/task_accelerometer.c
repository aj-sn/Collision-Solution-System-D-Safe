/*******************************************************************************
  MPLAB Harmony Application Source File

  File Name:
    task_accelerometer.c

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

#include "task_accelerometer.h"
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "CSSDefinitions.h"             // System specific defines
#include <stdint.h>
#include <string.h>

extern collisionFlags collision;

extern uint8_t accelString[256];
extern uint8_t tempAccelString[256];
extern uint16_t rxCounterAccel;
extern volatile uint32_t nBytesReadAccel;

//char buff[73] = "$XDATA,0.002*\n$XDATA,-0.001*\n$YDATA,0.003*\n$YDATA,-0.004*\n$ZDATA,-0.002*";
//char buff[73] = "$XDATA,0.002*\n$YDATA,-0.003*\n$ZDATA,-0.002*";
char Data[256];
char *string = NULL;
char *aData = NULL;
char arData[9];
char axis[2];
char val[7];
uint8_t stringCompleteFlag = 2;
char* token = NULL;
char* token2 = NULL;
char* token3 = NULL;
double fval = 0;
char *ptr = NULL;
uint8_t cnt = 0;
uint8_t dir = 2;
double xVal = 0;
double yVal = 0;
double zVal = 0;
uint8_t xDir = 2;
uint8_t yDir = 2;
uint8_t zDir = 2;

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
    This structure should be initialized by the TASK_ACCELEROMETER_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

TASK_ACCELEROMETER_DATA task_accelerometerData;

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
    char *strseperate(char **, const char *)

  Remarks:
    String separator function using specified delimiter
 */

char *strseperate(char **stringp, const char *delim) 
{
    char *rv = *stringp;
    if (rv) {
        *stringp += strcspn(*stringp, delim);
        if (**stringp)
            *(*stringp)++ = '\0';
        else
            *stringp = 0; }
    return rv;
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void TASK_ACCELEROMETER_Initialize ( void )

  Remarks:
    See prototype in task_accelerometer.h.
 */

void TASK_ACCELEROMETER_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    task_accelerometerData.state = TASK_ACCELEROMETER_STATE_INIT;
}


/******************************************************************************
  Function:
    void TASK_ACCELEROMETER_Tasks ( void )

  Remarks:
    See prototype in task_accelerometer.h.
 */

void TASK_ACCELEROMETER_Tasks ( void )
{   
    while(true)
    {   
        //SERCOM4_USART_Write((uint8_t*)'\0', 1);
        
        if(SERCOM4_USART_ReadCountGet() != 0)
        {   
            if(accelString[rxCounterAccel] == '\n')
            {   
                memset(tempAccelString, '\0', strlen((const char*)tempAccelString));
                strcpy((char*)tempAccelString, (char*)accelString);
                //printf("\ntempAccelString : %s\n", tempAccelString);

                if(((token = strstr((const char*)tempAccelString, "$")) != NULL) && (strstr((const char*)tempAccelString, "*") != NULL))
                {
                    cnt = 0;
                    //printf("\ntoken : %s\n", token);   
                    if(strlen((const char*)token) > 12)
                    {    
                    token2 = strtok(token, ",\n");
   
                        while(token2 != NULL)
                        {
                            //printf("\ntoken2 = %s \n", token2);

                            if(cnt == 0)
                            {
                                memset(axis, 0, sizeof(axis));
                                memcpy(axis, &token2[1], 1);
                                axis[1] = '\0';
                                cnt = 1;
                                //printf("axis: %s\n", axis);
                            }
                            else if(cnt == 1)
                            {

                                memset(val, 0, sizeof(val));
                                //printf("\ntoken2 = %s \n", &token2[]);
                                memcpy(val, &token2[0], (strlen(token2) - 1));
                                val[strlen(val) + 1] = '\0';
                                //printf("val : %s\n", val);
                                if(strstr(val, "-") != NULL)
                                {
                                    dir = 0;
                                    memcpy(val, &token2[1], (strlen(token2) - 1));
                                    val[strlen(val) + 1] = '\0';
                                }
                                else
                                {
                                    dir = 1;
                                }

                                fval = strtod(val, &ptr);
                                stringCompleteFlag = 1;
                                cnt = 0;
                            }
                            
                            if(stringCompleteFlag == 1)
                            {
                                switch(axis[0])
                                {
                                    case 'X':
                                        xVal = fval;
                                        xDir = dir;
                                        //printf("xVal : %lf , xDir : %d\n", xVal, xDir);
                                        break;

                                    case 'Y':
                                        yVal = fval;
                                        yDir = dir;
                                        //printf("yVal : %lf , yDir : %d\n", yVal, yDir);
                                        break;

                                    case 'Z':
                                        zVal = fval;
                                        zDir = dir;
                                        //printf("zVal : %lf , zDir : %d\n", zVal, zDir);
                                        break;
                                }

                                //printf("yVal : %lf , yDir : %d\n", yVal, yDir);  
                                if(yVal >= 0.4)
                                {
                                    collision.accelerometer = 1;
                                }
                                
                                stringCompleteFlag = 0;
                            }

                            token2 = strtok(NULL,",\n");
                            //printf("token %s \n", token);
                            //Yield task?
                        }
                    }
                    else
                    {
                        printf("Corrupted data\n");
                    }
                }
                else
                {
                    printf("\n string error\n");
                }
                memset(accelString, '\0', strlen((const char*)accelString));
            }
            else
            {   
                rxCounterAccel++;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);  
    }
}


/*******************************************************************************
 End of File
 */
