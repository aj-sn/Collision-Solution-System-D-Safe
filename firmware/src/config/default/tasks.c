/*******************************************************************************
 System Tasks File

  File Name:
    tasks.c

  Summary:
    This file contains source code necessary to maintain system's polled tasks.

  Description:
    This file contains source code necessary to maintain system's polled tasks.
    It implements the "SYS_Tasks" function that calls the individual "Tasks"
    functions for all polled MPLAB Harmony modules in the system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    polled in the system.  These handles are passed into the individual module
    "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"


// *****************************************************************************
// *****************************************************************************
// Section: RTOS "Tasks" Routine
// *****************************************************************************
// *****************************************************************************
/* Handle for the TASK_ACCELEROMETER_Tasks. */
TaskHandle_t xTASK_ACCELEROMETER_Tasks;

void _TASK_ACCELEROMETER_Tasks(  void *pvParameters  )
{   
    while(1)
    {
        TASK_ACCELEROMETER_Tasks();
    }
}
/* Handle for the TASK_RANGEFINDER_Tasks. */
TaskHandle_t xTASK_RANGEFINDER_Tasks;

void _TASK_RANGEFINDER_Tasks(  void *pvParameters  )
{   
    while(1)
    {
        TASK_RANGEFINDER_Tasks();
    }
}
/* Handle for the TASK_GPSGET_Tasks. */
TaskHandle_t xTASK_GPSGET_Tasks;

void _TASK_GPSGET_Tasks(  void *pvParameters  )
{   
    while(1)
    {
        TASK_GPSGET_Tasks();
    }
}
/* Handle for the TASK_GSMDISPATCH_Tasks. */
TaskHandle_t xTASK_GSMDISPATCH_Tasks;

void _TASK_GSMDISPATCH_Tasks(  void *pvParameters  )
{   
    while(1)
    {
        TASK_GSMDISPATCH_Tasks();
    }
}




// *****************************************************************************
// *****************************************************************************
// Section: System "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SYS_Tasks ( void )

  Remarks:
    See prototype in system/common/sys_module.h.
*/
void SYS_Tasks ( void )
{
    /* Maintain system services */
    

    /* Maintain Device Drivers */
    

    /* Maintain Middleware & Other Libraries */
    

    /* Maintain the application's state machine. */
        /* Create OS Thread for TASK_ACCELEROMETER_Tasks. */
    xTaskCreate((TaskFunction_t) _TASK_ACCELEROMETER_Tasks,
                "TASK_ACCELEROMETER_Tasks",
                375,
                NULL,
                4,
                &xTASK_ACCELEROMETER_Tasks);

    /* Create OS Thread for TASK_RANGEFINDER_Tasks. */
    xTaskCreate((TaskFunction_t) _TASK_RANGEFINDER_Tasks,
                "TASK_RANGEFINDER_Tasks",
                256,
                NULL,
                3,
                &xTASK_RANGEFINDER_Tasks);

    /* Create OS Thread for TASK_GPSGET_Tasks. */
    xTaskCreate((TaskFunction_t) _TASK_GPSGET_Tasks,
                "TASK_GPSGET_Tasks",
                500,
                NULL,
                1,
                &xTASK_GPSGET_Tasks);

    /* Create OS Thread for TASK_GSMDISPATCH_Tasks. */
    xTaskCreate((TaskFunction_t) _TASK_GSMDISPATCH_Tasks,
                "TASK_GSMDISPATCH_Tasks",
                625,
                NULL,
                2,
                &xTASK_GSMDISPATCH_Tasks);




    /* Start RTOS Scheduler. */
    
     /**********************************************************************
     * Create all Threads for APP Tasks before starting FreeRTOS Scheduler *
     ***********************************************************************/
    vTaskStartScheduler(); /* This function never returns. */

}

/*******************************************************************************
 End of File
 */

