/*******************************************************************************
  MPLAB Harmony Application Source File

  File Name:
    task_gpsget.c

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

#include "task_gpsget.h"
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "CSSDefinitions.h"             // System specific defines
#include <stdint.h>
#include <string.h>

extern collisionFlags collision;

extern uint8_t gpsString[256];
extern uint8_t tempGpsString[256];
extern uint16_t rxCounterGPS;
extern volatile uint32_t nBytesReadGPS;

uint8_t buff[255] = "$GPGLL,4523.17057,N,07023.75876,W,170803.00,A,A*7C\n$SSSSL,3923.12975,N,21158.51716,W,239229.48,A,A*41\n$GPGLL,4523.17475,N,28158.11416,W,221229.48,A,A*41\n";

char gpsData[255];
char *gpgllString = NULL;

uint8_t nmeaString[80];
char  *rawSum = NULL;
char checkSum[3];
int int_CheckSum = 0;
char hex_CheckSum[3];
int cntGPS = 0;
char * pV = NULL;
uint8_t stringComplteFlag = 0;

char latRaw[15] = {};
char latDeg[3] = {};
char latMinSec[9] = {};
char dirNS[2] = {};

char lonRaw[15] = {};
char lonDeg[4] = {};
char lonMinSec[9] = {};
char dirEW[2] = {};

char utcRaw[15] = {};

char hH[3];
char mM[3];
char sS[3];

char location[50];
double latDecimal = 0;
double lonDecimal = 0;
char *dirLat = NULL;
char *dirLon = NULL;
char *ptr1 = NULL, *ptr2 = NULL;
char gmLink[70];

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
    This structure should be initialized by the TASK_GPSGET_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

TASK_GPSGET_DATA task_gpsgetData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* Function to verify checksum
 * Param : string (NMEA Data starting with GPGLL
 * XOR the first character with the next character, until the end of the string.
 * return the final XOR-ed value */
int verifyCheckSum(uint8_t* string)
{
	char *token, check = 0;
	int size = 0;

	token = strtok((char *)string, "*");
	size = strlen(token);
	for(int i = 1; i < size; i++)
	{
		check = check ^ token[i];
	}

	return check;
}



// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void TASK_GPSGET_Initialize ( void )

  Remarks:
    See prototype in task_gpsget.h.
 */

void TASK_GPSGET_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    task_gpsgetData.state = TASK_GPSGET_STATE_INIT;
    printf("\nGPS init\n");
    
}


/******************************************************************************
  Function:
    void TASK_GPSGET_Tasks ( void )

  Remarks:
    See prototype in task_gpsget.h.
 */

void TASK_GPSGET_Tasks ( void )
{  
    while(true)
    {
        if(SERCOM0_USART_ReadCountGet() != 0)
        {   
            if(gpsString[rxCounterGPS] == '\n')
            {   
                memset(tempGpsString, '\0', strlen((const char*)tempGpsString));
                strcpy((char*)tempGpsString, (char*)gpsString);
                //printf("\n%s", tempGpsString);
            }
            else
            {   
                rxCounterGPS++;
            }
        }
        
        //Parsing code begin//////////////////////////////////////////////////////////////       
        memset(gpsData, 0 , sizeof(gpsData));     //Clearing the buffer
        sprintf(gpsData, "%s", tempGpsString);		//Copying the data received from UART(assumption) to a new string variable
        //printf("\n%s\n", gpsData);
        
        gpgllString = strstr(gpsData, "$");
        if(((gpgllString = strstr((const char *)gpgllString, "$GPGLL")) != 0) && (strlen((const char *)gpgllString) > 49) &&
            (strstr((const char *)gpgllString, "*") != 0))     //Filtering strings starting with GPGLL and Verifying the validity of the same
        {
            memset(nmeaString, 0, sizeof(nmeaString));
            memcpy(nmeaString, &gpgllString[0], strlen(gpgllString));
            nmeaString[strlen((const char*)nmeaString) + 1] = '\0';
            //printf("\nnmeaString\n%s\n", nmeaString);  
            rawSum = strstr((const char *)nmeaString,(const char *) "*"); //Obtaining the string up to '*' character
            memset(checkSum, 0, sizeof(checkSum));						  // Clearing the string variable
            memcpy(checkSum, &rawSum[1], 2);
            checkSum[2] = '\0';
            //printf("\nReceived checksum = %s\n", checkSum);
            int_CheckSum = verifyCheckSum(nmeaString);					  // Calling verify checksum function
            sprintf(hex_CheckSum,"%X", int_CheckSum);					  // Converting the value received from the function to hex value to get the checksum
            //printf("\nCalculated check sum is %s\n", hex_CheckSum);

            if(strstr(hex_CheckSum, checkSum) != NULL)					  // Verifying the checksum received with the calculated checksum value
            {
                //printf("checksum verified\n");
                cntGPS = 0;
                stringComplteFlag = 0;
                pV = strtok((char*)nmeaString, ",\n");
                while(pV != NULL && stringComplteFlag == 0) // Parsing the string to get Latitude and longitude with directions and the UTC
                {   
                     //printf("pV:\n %s\n", pV);
                    switch (cntGPS)
                    {
                       case 1: 
                            memset(latRaw, 0, sizeof(latRaw));
                            memcpy(latRaw, pV, strlen((const char *)pV)); //Copying parsed latitude token
                            latRaw[strlen((const char *)pV) + 1] = '\0';
                            if(strcmp(latRaw, "") == 0)
                            {
                                printf("\nMemcpy failed!\n");
                            }

                            //printf("Lat: %s\n", latRaw);  			
                            break;

                        case 2:
                            memset(dirNS, 0, sizeof(dirNS));
                            memcpy(dirNS, pV, strlen((const char *)pV)); //Copying parsed latitude token
                            dirNS[1] = '\0';
                            if(strcmp(dirNS, "") == 0)
                            {
                                printf("\nMemcpy failed!\n");
                            }
                            
                            //printf("dirNS: %s\n", dirNS);    		
                            break;

                        case 3:
                            memset(lonRaw, 0, sizeof(lonRaw));
                            memcpy(lonRaw, pV, strlen((const char *)pV)); //Copying parsed latitude token
                            lonRaw[strlen((const char *)pV) + 1] = '\0';
                            if(strcmp(lonRaw, "") == 0)
                            {
                                printf("\nMemcpy failed!\n");
                            }
                            
                            //printf("lonRaw: %s\n", lonRaw);	
                            break;

                        case 4:
                            memset(dirEW, 0, sizeof(dirEW));
                            memcpy(dirEW, pV, strlen((const char *)pV)); //Copying parsed latitude token
                            dirEW[1] = '\0';
                            if(strcmp(dirEW, "") == 0)
                            {
                                printf("\ndirEW Memcpy failed!\n");
                            }
                            
                            //printf("dirEW: %s\n", dirEW);		
                            break;

                        case 5:
                            memset(utcRaw, 0, sizeof(utcRaw));
                            memcpy(utcRaw, pV, strlen((const char *)pV)); //Copying parsed latitude token
                            utcRaw[strlen((const char *)pV) + 1] = '\0';
                            if(strcmp(utcRaw, "") == 0)
                            {
                                printf("\nMemcpy failed!\n");
                            }
                            
                            //printf("utcRaw: %s\n", utcRaw);   	
                            break;
                                    
                        case 6:
                            stringComplteFlag = 1;
                            break;
                                    
                        default:
                            break;                                   
                                    
                    }

                    cntGPS++;
                    pV = strtok(NULL, ",\n"); 
                   
                }
                
                if(stringComplteFlag == 1)
                {
                    //parcing latitude degrees, minitutes and seconds from the raw value                                 
                    memcpy(latDeg, &latRaw[0], 2);
                    latDeg[2] = '\0';
                    //printf("LatDeg: %s\n",latDeg);

                    memcpy(latMinSec, &latRaw[2], 7);
                    latMinSec[7] = '\0';
                    //printf("LatMinSec: %s\n",latMinSec);
                    

                    //parcing longitude degrees, minitutes and seconds from the raw value
                    memcpy(lonDeg, &lonRaw[0], 3);
                    lonDeg[3] = '\0';
                    
                    memcpy(lonMinSec, &lonRaw[3], 7);
                    lonMinSec[8] = '\0';

                    /* Storing N and E directions as '-' and S and W as '+' */
                    dirLat = (strcmp(dirNS, "N") == 0) ? "+" : "-";
                    dirLon = (strcmp(dirEW, "E") == 0) ? "+" : "-";

                    //parcing utc time as hours minutes and seconds
                    memcpy(hH, &utcRaw[0], 2);
                    hH[2] = '\0';

                    memcpy(mM, &utcRaw[2], 2);
                    mM[2] = '\0';

                    memcpy(sS, &utcRaw[4], 2);
                    sS[2] = '\0';

                    //Storing the location data in "N/S Latitude�MinutesSec' E/W Longitude�MinutesSec' UTC : hh:mm:ss" format
                    memset(location, 0, sizeof(location));
                    sprintf(location, "%s %sDeg%s\', %s %sDeg%s\', UTC: %s:%s:%s",dirNS, latDeg, latMinSec, dirEW, lonDeg, lonMinSec, hH, mM, sS);
                    //printf("%s\n", location);
                    
                    /* Converting the latitude and longitude to decimal values to suit the google map link format*/

                    latDecimal = (strtod(latDeg, &ptr2) + ((strtod(latMinSec, &ptr1))/60));
                    ptr1 = ptr2 = NULL;
                    lonDecimal = (strtod(lonDeg, &ptr2) + ((strtod(lonMinSec, &ptr1))/60));
                    ptr1 = ptr2 = NULL;

                    /*Adding Data to the google map link format*/
                    memset(gmLink, 0, sizeof(gmLink));
                    /*Storing the google map link in a variable*/
                    sprintf(gmLink, "https://www.google.com/maps/search/?api=1&query=%s%lf,%s%lf", dirLat, latDecimal, dirLon, lonDecimal);
                    //printf("Googla Map Link : %s\n\n", gmLink); 
                    
                    collision.gpsReady = 1;
                }
            }
            else
            {
                //printf("\nWrong Checksum ! Tampered Data\n");
                collision.gpsReady = 0;
            }
        }
        else
        {
            //printf("\n...\n");
            collision.gpsReady = 0;
        }
        //Parsing code end   ////////////////////////////////////////////////////////////
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}


/*******************************************************************************
 End of File
 */
