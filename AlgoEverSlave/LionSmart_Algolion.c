/*-----------------------------------------------------------------------------
 * Name:		LionSmart_Algolion.c
 * Purpose:		Implements LIONSmart(C) interface
 * Revision:	02.00
 * Author:		Anatoly Kh.
 * Copyright:	(C) 2020 Algolion LTD. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "ev_common.h"

EV_ERROR_ITEM	e[EV_ERROR_STACK_COUNT];
EV_GLOBALS		g;
EV_CONFIG		c;

EV_TP_RESULTS	gResults;	// Results, buffer #0

EV_TP_HISTORY	gHistory;	// The history of results

static LMM_INTERFACE_SLAVE gInterfaceSlave;

static unsigned char	flagWasInitialized		= FALSE;	// Either the module was initialized
static unsigned char	flagShouldBeTerminated	= FALSE;	// Either the module should be terminated
static unsigned char	flagInterfaceProvided	= FALSE;	// Either the LionSMART interface was provided

void ALGO_SlaveSetupInterface(PCLMM_INTERFACE_SLAVE pInterface)
{
	if (pInterface->flashRead		!= NULL) gInterfaceSlave.flashRead		= pInterface->flashRead;
	if (pInterface->flashWrite		!= NULL) gInterfaceSlave.flashWrite		= pInterface->flashWrite;
	if (pInterface->getTime			!= NULL) gInterfaceSlave.getTime		= pInterface->getTime;
	if (pInterface->getVoltage		!= NULL) gInterfaceSlave.getVoltage		= pInterface->getVoltage;
	if (pInterface->getCurrent		!= NULL) gInterfaceSlave.getCurrent		= pInterface->getCurrent;
	if (pInterface->getTemperature	!= NULL) gInterfaceSlave.getTemperature	= pInterface->getTemperature;
	flagInterfaceProvided = TRUE;
}

#define EV_FLASH_PAGE		256 // Bytes. The size of portion to read/write FLASH memory
//static const unsigned char EV_FLASH_MAGIC[7] = { 0x29, 0xC8, 0x27, 0xC6, 0x25, 0xC4, 0x23 };


inline __attribute__((always_inline)) static
BOOL ev_flash_read_slave(void)
{
	unsigned short offset;
	unsigned char  rc = TRUE;
	unsigned char* p = (unsigned char*) &gHistory;

	for (offset = 0; rc && (offset < sizeof(EV_TP_HISTORY)); offset += EV_FLASH_PAGE)
	{
		rc = gInterfaceSlave.flashRead(offset, p + offset, EV_FLASH_PAGE);
	}
	return rc;
}

inline __attribute__((always_inline)) static
BOOL ev_flash_write_slave(void)
{
	unsigned short offset;
	unsigned char  rc = TRUE;
	unsigned char* p = (unsigned char*) &gHistory;

	for (offset = 0; rc && (offset < sizeof(EV_TP_HISTORY)); offset += EV_FLASH_PAGE)
	{
		rc = gInterfaceSlave.flashWrite(p + offset, EV_FLASH_PAGE);
	}
	return rc;
}


uint8_t  ALGO_SlaveInitialize(void)
{
	if (!flagInterfaceProvided || flagWasInitialized ||
		(gInterfaceSlave.flashRead		== NULL) ||
		(gInterfaceSlave.flashWrite		== NULL) ||
		(gInterfaceSlave.getTime		== NULL) ||
		(gInterfaceSlave.getVoltage		== NULL) ||
		(gInterfaceSlave.getCurrent		== NULL) ||
		(gInterfaceSlave.getTemperature	== NULL))
		return ALGO_ERROR_INTEFACE;

	if (!ev_check_mcu_id())
		return ALGO_ERROR_SECURITY;

	ev_init_slave();
	ev_flash_read_slave();
	flagWasInitialized = TRUE;
	flagShouldBeTerminated = FALSE;

	return 0;
}


void ALGO_SlaveTerminate(void)
{
	// If [EV_RAW_DATA_SAMPLE.rs_time] is zero, it means
	// the current TP, if any exists, is over and should be calculated
	//EV_RAW_DATA_SAMPLE raw = { 0 };

	if (!flagWasInitialized || !flagInterfaceProvided)
	{
		//EV_ERROR3(EV_ERROR_MODULE_INITIALIZE);
		return;
	}

	ULONG countSampleMax = g.uCountSample + 10;
	while ((g.offsetSkipCheck <= 0) && (g.uCountSample < countSampleMax))
	{
		g.uCountSample++;
		ev_select_tp();
	}
	ev_flash_write_slave();
	flagShouldBeTerminated = TRUE;
	flagInterfaceProvided = FALSE;
	flagWasInitialized = FALSE;
}


void ALGO_SlaveEventTrigger(void)
{
	EV_RAW_DATA_SAMPLE raw;
	uint16_t* pVoltageArray;
	BOOL valid;

	if (!flagWasInitialized || !flagInterfaceProvided || flagShouldBeTerminated)
	{
		//EV_ERROR3(EV_ERROR_MODULE_INITIALIZE);
		return;
	}

	ev_zeromem(&raw, sizeof(EV_RAW_DATA_SAMPLE));

	// Get measurements:
	raw.rs_time = gInterfaceSlave.getTime() * 1000; // to microseconds
	raw.current	= gInterfaceSlave.getCurrent();
	pVoltageArray = gInterfaceSlave.getVoltage();

	valid = ABS(raw.current) < c.maxCurrent * EV_PACK_CELLS_INSUPER;
	for (BYTE i = 0; valid && (i < EV_PACK_SUPERS_INMODULE); i++)
	{
		raw.voltagea[i] = pVoltageArray[i]; // Casting
		valid = (raw.voltagea[i] >= c.minVoltage) &&
				(raw.voltagea[i] <= c.maxVoltage);
	}

	if (valid)
		ev_select(&raw);
	//else
		//EV_ERROR3(EV_ERROR_RAW_DATA_INVALID);
}

const unsigned char* ALGO_SlaveGetSoS(void)
{
	return gResults.tp_rank;
}


void ev_error_handler(const LONG _code, const BYTE _type, const BYTE _severity,
	const char* _source, const LONG _line, const ULONGLONG _time, const CHAR* _text)
{
}
