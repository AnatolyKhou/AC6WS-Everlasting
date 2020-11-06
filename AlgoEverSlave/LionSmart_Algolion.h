/*-----------------------------------------------------------------------------
 * Name:		LionSmart_Algolion.h
 * Purpose:		LIONSmart(C) interface for "Slave" and "Master" MCU
 * Revision:	02.00
 * Author:		Anatoly Kh.
 * Copyright:	(C) 2020 Algolion LTD. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef LIONSMART_ALGOLION_H
#define LIONSMART_ALGOLION_H

/*
	Global macros
	=============

	This file requires one or both Preprocessor Symbols (MACROs) definitions:
	EV_MASTER		- to produce the interface for Master MCU
	EV_SLAVE		- to produce the interface for Slave  MCU

	They should be define as:
	#define EV_MASTER 1
	-or-
	#define EV_SLAVE 1
	-or-
	pointed in the tab "C/C++ (AC6)" in the dialog box "Configure Flash Tools..."
*/

#ifndef EV_MASTER
#ifndef EV_SLAVE
#error "Both macros \"EV_MASTER\" and \"EV_SLAVE\" are undefined"
#endif
#endif


/*
	Types definitions
	=================

	All types, that is used in this file, are defined here
*/

#ifndef int32_t
typedef long int int32_t;
#endif
#ifndef uint32_t
typedef unsigned long int uint32_t;
#endif
#ifndef uint16_t
typedef unsigned short uint16_t;
#endif
#ifndef uint8_t
typedef unsigned char uint8_t;
#endif


#ifdef EV_SLAVE
/*
	 1. LIONSmart functions patterns:
	 ================================
*/

/*
	Original name is LMM_GetMeasTimeStamp
	Time measurements
	Return:
		Timestamp - 32 bits unsigned integer value, units milliseconds.
		It is relative time. Starts from 0 after system reset.
		Voltage and current measurements are synchronized.
*/
typedef uint32_t (*LMM_GET_TIME) (void);


/*
	Original name is LMM_GetCellVoltagesPerModule
	Voltage measurements
	Return:
		Pointer to array of 12 unsigned 16 bits values, unit is [100 uV].
*/
typedef uint16_t* (*LMM_GET_VOLTAGE) (void);

/*
	Original name is LMM_GetCurrentPerModule.
	Current measurements
	Return:
		Signed 32 bits integer value, units [100uA].
		Charge current is positive and discharge is negative.
*/
typedef int32_t (*LMM_GET_CURRENT) (void);


/*
	Original name is LMM_GetCellTemperaturePerModule.
	Temperature measurements
	Return:
		Pointer to array of unsigned 16 bits integers of 8 values, unit is [centi Kelvin].
*/
typedef uint16_t* (*LMM_GET_TEMPERATURE) (void);
#endif


/*************************************************************************//**
* @brief	Write data into the flash memory
*			Original name is FM_ALGO_WriteToFlashMemory
*
* @param	data		- pointer with data to be written on the flash memory
* @param	dataLength	- number of bytes to be written on to the flash memory
* @return
*			FALSE if dataLength is greater than page size (256 bytes) or,
*				  if memory full received from FM_MemoryManager.
*			TRUE  if succeeded
*****************************************************************************/
typedef uint8_t (*LMM_FLASH_WRITE_BYTES) (uint8_t *data, uint32_t dataLength);


/*****************************************************************************
* @brief	Read data from the flash memory
*			Original name is FM_ALGO_ReadFromFlashMemory
*
* @details	Page roll-over is taken care in this function. Any size of data smaller
*			than available free memory can be written at one function call.
* @param	address		- Start address of the memory location to be read from
*						  Address starts from 0x000000 (offset is already set in the  function)
*						  The end of the memory limit would be provided to you soon.
* @param	data		- The data read from address location of the flash memory written into data pointer
* @param	dataLength	- number of bytes to be Read from the flash memory
* @return
*			FALSE if the memory location requested for access is out of Memory
*         		  manager authorization limit
*			TRUE  if succeeded
*****************************************************************************/
typedef uint8_t (*LMM_FLASH_READ_BYTES) (uint32_t address, uint8_t *data, uint32_t dataLength);


/*
	To send data(up to 1Kbyte) via CAN to the Master MCU Algolion module:
	HAL_StatusTypeDef CAN_TxMsg(uint32_t ID, CAN_Payload_t* data, uint8_t DLC);
	Where:  ID  = 0x700 (dummy ID)
			DLC = Length of data, 8 bytes maximum

	typedef union __attribute__((packed)) {
		uint64_t Dword;
		uint32_t Word [2];
		uint16_t Hword[4];
		uint8_t  Byte [8];
	} CAN_Payload_t;

	To send resulted alert (Integrated state of safety for the battery pack)
*/


/*
	The 1st parameter "report", when an external context calls
	Used in function:
		void ALGO_CanSlaveReport(const void* report, uint8_t length);
*/
typedef struct LMM_CAN_MESSAGE_tag
{
	uint32_t    id;				// Message Identifier
	uint8_t     data[8];		// Data buffer
	uint8_t     length;			// Data length (0-8)
	uint8_t     rtr;			// Remote Request Bit
	uint8_t     controllerNo;	// CAN controller No. The message was received or should be transmitted starting with 1
} __attribute__((packed))
LMM_CAN_MESSAGE;


#ifdef EV_MASTER
typedef struct LMM_INTERFACE_MASTER_tag
{
	LMM_FLASH_READ_BYTES	flashRead;		// LIONSmart function "void LMM_FlashReadBytes(unsigned char* pBytes, unsigned short length);"
	LMM_FLASH_WRITE_BYTES	flashWrite;		// LIONSmart function "void LMM_FlashWriteBytes(const unsigned char* pBytes, unsigned short length);"
	unsigned int			canControlId;	// 1-st parameter for the function "CAN_TxMsg"; From master to vehicle
	unsigned int			reserved;		// Reserved, should be NULL
}
LMM_INTERFACE_MASTER, *PLMM_INTERFACE_MASTER;
typedef const LMM_INTERFACE_MASTER *PCLMM_INTERFACE_MASTER;
#endif // EV_MASTER


#ifdef EV_SLAVE
typedef struct LMM_INTERFACE_SLAVE_tag
{
	LMM_GET_TIME			getTime;		// LIONSmart function "uint32_t  LMM_GetMeasTimeStamp			(void);"
	LMM_GET_VOLTAGE			getVoltage;		// LIONSmart function "uint16_t* LMM_GetCellVoltagesPerModule	(void);"
	LMM_GET_CURRENT			getCurrent;		// LIONSmart function "int32_t   LMM_GetCurrentPerModule		(void);"
	LMM_GET_TEMPERATURE		getTemperature;	// LIONSmart function "uint16_t* LMM_GetCellTemperaturePerModule(void);"
	LMM_FLASH_READ_BYTES	flashRead;		// LIONSmart function "uint8_t   FM_ALGO_ReadFromFlashMemory	(uint32_t address, uint8_t *data, uint32_t dataLength);"
	LMM_FLASH_WRITE_BYTES	flashWrite;		// LIONSmart function "uint8_t   FM_ALGO_WriteToFlashMemory		(uint8_t *data, uint32_t dataLength);"
	unsigned int			canControlId;	// 1-st parameter for the function "CAN_TxMsg"; From slave to master
	unsigned int			reserved;		// Reserved, should be NULL
}
LMM_INTERFACE_SLAVE, *PLMM_INTERFACE_SLAVE;

typedef const LMM_INTERFACE_SLAVE *PCLMM_INTERFACE_SLAVE;
#endif // EV_SLAVE


#ifdef EV_SLAVE
/*
	2. Algolion slave module interface:
	===================================
*/

/*
	Setup the interface to LIONSmart functions
	Should be first function to call
	Parameters:
		pInterface - pointer to a structure PCLMM_INTERFACE_SLAVE
					 all members should be not NULL (zero)
*/
void ALGO_SlaveSetupInterface(PCLMM_INTERFACE_SLAVE pInterface);

/*
	Initialize the module
	Should be second function to call
	(first one is ALGO_SlaveSetupInterface)

	Return:
		Zero, if the module was initialized successfully
		Error code, if the module failed.

	Possible error code values are ALGO_ERROR_***.
*/
uint8_t ALGO_SlaveInitialize(void);

#define ALGO_ERROR_INTEFACE		0x01 // The interface (see function ALGO_SlaveSetupInterface) structure is wrong
#define ALGO_ERROR_SECURITY		0x02 // The security violation

/*
	De-initialize the module,
	releases all resources, save data to non-volatile memory.
	Should be last function to call (next call is "ALGO_SlaveSetupInterface")
*/
void ALGO_SlaveTerminate(void);

/*
	The BMS measures voltage and current of the battery;
	Just after measurements (when new voltage and current data became available),
	this function should be called.
	For example, if BMS measures voltage and current with frequency 100Hz,
	it should called this function with same frequency.

	The function calls functions LMM_GET_TIME, LMM_GET_VOLTAGE, LMM_GET_CURRENT, and LMM_GET_TEMPERATURE
	and saves new sample (time-voltage-current) in the internal buffer.

	If the internal buffer contains a "Transition Period", the function ALGO_SlaveEventTrigger
	starts separate thread to calculates a “State-of-Safety” of the battery.
	CPU time of this thread is not greater than 100 milliseconds.
*/
void ALGO_SlaveEventTrigger(void);

/*
	Get array of “State-of-Safety” values; “State-of-Safety” can be 0, 1, 2, and 3.
	One value per one supercell; totally 12 values.
	The order of supercells in the array is same as in function LMM_GET_VOLTAGE.
	Possible values:
		0	ALGO_CELL_UNKNOWN		“State-of-Safety” was never calculated before
		1	ALGO_CELL_NORMAL		A supercell is in normal condition
		2	ALGO_CELL_ATTENSION		A supercell is in dangerous; requires attention
		3	ALGO_CELL_CRITICAL		A supercell is in critical condition; should be removed

	The function can be called at any time.
	Recommended call frequency is 5 minutes.

	Return:
		Pointer to an array of 12 elements; each element is a 1-byte value.
*/
const uint8_t* ALGO_SlaveGetSoS(void);
#endif

/*
	Return values for functions "ALGO_GetSoS" and "ALGO_SlaveGetSoS"
	================================================================
	The values from 4 up to 255 are reserved and
	should be processed same way as "ALGO_CELL_UNKNOWN".
*/
#define ALGO_CELL_UNKNOWN		0	// “State-of-Safety” was never calculated before
#define ALGO_CELL_NORMAL		1	// A (super-)cell is in normal condition
#define ALGO_CELL_ATTENSION		2	// A (super-)cell is in dangerous; requires attention
#define ALGO_CELL_CRITICAL		3	// A (super-)cell is in critical condition; it should be replaced/repaired


#ifdef EV_MASTER
/*
	3. Algolion master module interface:
	====================================
*/

/*
	Setup the interface to LIONSmart functions.
	Should be first function to call
	Parameters:
		pInterface - pointer to a structure LMM_INTERFACE

	In the structure LMM_INTERFACE all members should be filled (not NULL)
*/
void ALGO_SetupInterface(PCLMM_INTERFACE_MASTER pInterface);

/*
	Initialize the module
	Should be second function to call
	(first one is ALGO_SetupInterface)
*/
void ALGO_Initialize(void);

/*
	De-initialize the module,
	releases all resources, save data to non-volatile memory.
	Should be last function to call (next call is "ALGO_SetupInterface")
*/
void ALGO_Terminate(void);

/*
	Receive CAN message.
	It is called when CAN message arrived from any slave module.
	Parameters:
		report	- pointer to a buffer that keeps CAN message
		length	- length of the buffer (less or equal to 8 bytes)

*/
void ALGO_CanSlaveReport(const void* report, uint8_t length);

/*
	Get last one “State-of-Safety” values for each cell.
	Return:
		Pointer to an array of 1 byte values;
		one value per cell; 12 * 16 = 192 values totally.
		See above (constants ALGO_CELL_***) for values
*/
const uint8_t* ALGO_GetSoS(void);

/*
	Reserved. Not used in the current version.
*/
void ALGO_DriverAction(uint8_t action);
#endif

#endif  // LIONSMART_ALGOLION_H
