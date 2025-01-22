/* 
 * File:   MMA8452Q.h
 * Author:
 *
 * Created on February 28, 2022, 5:58 PM
 */

#ifndef MMA8452Q_H
#define	MMA8452Q_H

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "CSSDefinitions.h"             // System specific defines
#include <stdint.h>

/* Provide C++ Compatibility */
//#ifdef __cplusplus
//extern "C" {
//#endif

///////////////////////////////////
// MMA8452Q Register Definitions //
///////////////////////////////////
enum MMA8452Q_Register
{
	STATUS_MMA8452Q = 0x00,
	OUT_X_MSB = 0x01,
	OUT_X_LSB = 0x02,
	OUT_Y_MSB = 0x03,
	OUT_Y_LSB = 0x04,
	OUT_Z_MSB = 0x05,
	OUT_Z_LSB = 0x06,
	SYSMOD = 0x0B,
	INTERRUPT_SOURCE = 0x0C,
	WHO_AM_I = 0x0D,
	XYZ_DATA_CFG = 0x0E,
	HP_FILTER_CUTOFF = 0x0F,
	PL_STATUS = 0x10,
	PL_CFG = 0x11,
	PL_COUNT = 0x12,
	PL_BF_ZCOMP = 0x13,
	P_L_THS_REG = 0x14,
	FF_MT_CFG = 0x15,
	FF_MT_SRC = 0x16,
	FF_MT_THS = 0x17,
	FF_MT_COUNT = 0x18,
	TRANSIENT_CFG = 0x1D,
	TRANSIENT_SRC = 0x1E,
	TRANSIENT_THS = 0x1F,
	TRANSIENT_COUNT = 0x20,
	PULSE_CFG = 0x21,
	PULSE_SRC = 0x22,
	PULSE_THSX = 0x23,
	PULSE_THSY = 0x24,
	PULSE_THSZ = 0x25,
	PULSE_TMLT = 0x26,
	PULSE_LTCY = 0x27,
	PULSE_WIND = 0x28,
	ASLP_COUNT = 0x29,
	CTRL_REG1 = 0x2A,
	CTRL_REG2 = 0x2B,
	CTRL_REG3 = 0x2C,
	CTRL_REG4 = 0x2D,
	CTRL_REG5 = 0x2E,
	OFF_X = 0x2F,
	OFF_Y = 0x30,
	OFF_Z = 0x31
};

////////////////////////////////
// MMA8452Q Misc Declarations //
////////////////////////////////
enum MMA8452Q_Scale
{
	SCALE_2G = 2,
	SCALE_4G = 4,
	SCALE_8G = 8
}; // Possible full-scale settings
enum MMA8452Q_ODR
{
	ODR_800,
	ODR_400,
	ODR_200,
	ODR_100,
	ODR_50,
	ODR_12,
	ODR_6,
	ODR_1
}; // possible data rates
// Possible portrait/landscape settings
#define PORTRAIT_U 0
#define PORTRAIT_D 1
#define LANDSCAPE_R 2
#define LANDSCAPE_L 3
#define LOCKOUT 0x40
#define MMA8452Q_DEFAULT_ADDRESS 0x1D

// Posible SYSMOD (system mode) States
#define SYSMOD_STANDBY 0b00
#define SYSMOD_WAKE 0b01
#define SYSMOD_SLEEP 0b10

////////////////////////////////
// MMA8452Q Class Declaration //
////////////////////////////////
#ifdef __cplusplus
class MMA8452Q
{
  public:
	MMA8452Q(uint8_t addr = MMA8452Q_DEFAULT_ADDRESS); // Constructor
	MMA8452Q_Scale scale;
	MMA8452Q_ODR odr;

	bool begin(uint8_t deviceAddress = MMA8452Q_DEFAULT_ADDRESS);
	uint8_t init(MMA8452Q_Scale fsr = SCALE_2G, MMA8452Q_ODR odr = ODR_800);
	void read();
	uint8_t available();
	uint8_t readTap();
	uint8_t readPL();
	uint8_t readID();

	short x, y, z;
	float cx, cy, cz;

	short getX();
	short getY();
	short getZ();

	float getCalculatedX();
	float getCalculatedY();
	float getCalculatedZ();

	bool isRight();
	bool isLeft();
	bool isUp();
	bool isDown();
	bool isFlat();

	void setScale(MMA8452Q_Scale fsr);
	void setDataRate(MMA8452Q_ODR odr);

  private:
	uint8_t _deviceAddress;   //Keeps track of I2C address. setI2CAddress changes this.

	void standby();
	void active();
	bool isActive();
	void setupPL();
	void setupTap(uint8_t xThs, uint8_t yThs, uint8_t zThs);
	void writeRegister(MMA8452Q_Register reg, uint8_t data);
	void writeRegisters(MMA8452Q_Register reg, uint8_t *buffer, uint8_t len);
	uint8_t readRegister(MMA8452Q_Register reg);
	void readRegisters(MMA8452Q_Register reg, uint8_t *buffer, uint8_t len);
};

#else
typedef struct MMA8452Q MMA8452Q;
#endif

// access functions
#ifdef __cplusplus
    #define EXPORT_C extern "C"
#else
    #define EXPORT_C
#endif

EXPORT_C MMA8452Q* MMA8452Q_new(void);
EXPORT_C bool MMA8452Q_begin(MMA8452Q* , uint8_t);
EXPORT_C uint8_t MMA8452Q_init(MMA8452Q* , uint8_t, uint8_t);
EXPORT_C uint8_t MMA8452Q_readID(MMA8452Q* );
EXPORT_C short MMA8452Q_getX(MMA8452Q* );
EXPORT_C short MMA8452Q_getY(MMA8452Q* );
EXPORT_C short MMA8452Q_getZ(MMA8452Q* );
EXPORT_C float MMA8452Q_getCalculatedX(MMA8452Q* );
EXPORT_C float MMA8452Q_getCalculatedY(MMA8452Q* );
EXPORT_C float MMA8452Q_getCalculatedZ(MMA8452Q* );
EXPORT_C void MMA8452Q_read(MMA8452Q* );
EXPORT_C uint8_t MMA8452Q_available(MMA8452Q* );
EXPORT_C void MMA8452Q_setScale(MMA8452Q* , uint8_t);
EXPORT_C void MMA8452Q_setDataRate(MMA8452Q* , uint8_t);
EXPORT_C uint8_t MMA8452Q_readTap(MMA8452Q* );
EXPORT_C uint8_t MMA8452Q_readPL(MMA8452Q* );
EXPORT_C bool MMA8452Q_isRight(MMA8452Q* );
EXPORT_C bool MMA8452Q_isLeft(MMA8452Q* );
EXPORT_C bool MMA8452Q_isUp(MMA8452Q* );
EXPORT_C bool MMA8452Q_isDown(MMA8452Q* );
EXPORT_C bool MMA8452Q_isFlat(MMA8452Q* );


    /* Provide C++ Compatibility */
//#ifdef __cplusplus
//}
//#endif

#endif	/* MMA8452Q_H */

