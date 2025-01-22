/* ************************************************************************** */
/** Collision Solution System project specific definitions header

  @File Name
    CSSDefinitions.h

  @Summary
    project specific definitions header.

  @Description
    project specific definitions header.
 */
/* ************************************************************************** */

#ifndef CSS_DEFINITIONS_H    /* Guard against multiple inclusion */
#define CSS_DEFINITIONS_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/*Definitions for constants will go here*/
#define PANIC_WAIT_THRESHOLD 3

/*Collision status flags*/
typedef union
{
    struct
    {
        uint8_t accelerometer : 1;
        uint8_t rangeFinder   : 1;
        uint8_t PanicState    : 1;
        uint8_t gpsReady      : 1;
        uint8_t reserved      : 4;  //padding to make it a full byte
    };
    uint8_t flagByte;   //access all flags as a single byte
}collisionFlags;

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* CSS_DEFINITIONS_H */

/* *****************************************************************************
 End of File
 */
