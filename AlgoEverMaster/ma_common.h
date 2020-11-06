/*-----------------------------------------------------------------------------
 * Name:		ma_common.h
 * Purpose:		Common header for all *.c files of the project
 * Revision:	02.00
 * Author:		Anatoly Kh.
 * Copyright:	(C) 2020 Algolion LTD. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef MA_COMMON_H
#define MA_COMMON_H

#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#define EV_MASTER 1
#include "./../AlgoEverSlave/LionSmart_Algolion.h"	// LIONSmart(C) interface for "Slave" and "Master" MCU

#include "./../AlgoEverSlave/ev_algo.h"            // Common header for pure Algolion staff


/**
  *	Results of calculation to be send to Master MCU via CAN bus
  * - time and set of the parameters
  */
typedef struct EV_MASTER_RES_tag
{
	int				sd_index;		// Index of Transition Period
	unsigned int	sd_time;		// Timestamp
	unsigned char	sd_soh[EV_PACK_SUPERS_INMODULE]; // SoH per Super-Cell, see ALGO_CELL_***
}
EV_MASTER_RES, *PEV_MASTER_RES;

extern EV_MASTER_RES gMasterRes;
extern EV_MASTER_RES gMasterList[EV_PACK_MODULES_INPACK]; // SoH per Super-Cell in Master module


//#include "environment.h"

#endif /* MA_COMMON_H */
