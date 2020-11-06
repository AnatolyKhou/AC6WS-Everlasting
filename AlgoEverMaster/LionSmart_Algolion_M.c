/*-----------------------------------------------------------------------------
 * Name:		LionSmart_Algolion_M.c
 * Purpose:		Implements LIONSmart(C) interface
 * Revision:	02.00
 * Author:		Anatoly Kh.
 * Copyright:	(C) 2020 Algolion LTD. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "ma_common.h"

LMM_INTERFACE_MASTER gInterfaceMaster;
EV_MASTER_RES gMasterRes;
EV_MASTER_RES gMasterList[EV_PACK_MODULES_INPACK]; // SoH per Super-Cell in Master module

static unsigned char	flagWasInitialized		= FALSE;	// Either the module was initialized
static unsigned char	flagShouldBeTerminated	= FALSE;	// Either the module should be terminated
static unsigned char	flagInterfaceProvided	= FALSE;	// Either the LionSMART interface was provided

/**
  *	@brief Initialize global Data Acquisition Configuration
  *		   in the Master module
  */

//INLINE
void ev_init_master(void)
{
	//ev_init_config();
}



void ALGO_SetupInterface(PCLMM_INTERFACE_MASTER pInterface)
{
	if ((pInterface->flashRead == NULL) ||
		(pInterface->flashWrite == NULL) ||
		flagInterfaceProvided)
	{
		EV_ERROR3(EV_ERROR_MODULE_INITIALIZE);
		return;
	}

	memcpy(&gInterfaceMaster, pInterface, sizeof(LMM_INTERFACE_MASTER));
	flagInterfaceProvided = TRUE;
}


void ALGO_Initialize(void)
{
	if (!flagInterfaceProvided || flagWasInitialized)
	{
		EV_ERROR3(EV_ERROR_MODULE_INITIALIZE);
		return;
	}
	ev_init_master();
	flagWasInitialized = TRUE;
	flagShouldBeTerminated = FALSE;
	gInterfaceMaster.flashRead(0, (PBYTE) gMasterList, sizeof(gMasterList));
}


void ALGO_Terminate(void)
{
	if (!flagWasInitialized || !flagInterfaceProvided)
	{
		EV_ERROR3(EV_ERROR_MODULE_INITIALIZE);
		return;
	}

	gInterfaceMaster.flashWrite((PBYTE) gMasterList, sizeof(gMasterList));
	flagShouldBeTerminated = TRUE;
	flagInterfaceProvided = FALSE;
	flagWasInitialized = FALSE;
}

void ALGO_CanSlaveReport(const void* report, unsigned char length)
{
	unsigned char* pData;
	unsigned short offset, index;

	if (!flagWasInitialized)
	{
		EV_ERROR3(EV_ERROR_MODULE_INITIALIZE);
		return;
	}
	if ((length < 3) || (length > 8))
	{
		EV_ERROR3(EV_ERROR_CAN_DATA);
		return;
	}

	pData = (unsigned char*) report;
	index = pData[0];
	offset = pData[1] * 6;

	memcpy(gMasterList[index].sd_soh + offset, pData + 2, 6);
}


const unsigned char* ALGO_GetSoS(void)
{
	return gMasterList[0].sd_soh;
}

void ALGO_DriverAction(unsigned char action)
{
	UNUSED(action);
}



