/*-----------------------------------------------------------------------------
 * Name:		ev_common.h
 * Purpose:		Common header for all *.c files of the project
 * Revision:	02.00
 * Author:		Anatoly Kh.
 * Copyright:	(C) 2020 Algolion LTD. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#define EV_SLAVE 1
#include "LionSmart_Algolion.h"	// LIONSmart(C) interface for "Slave" and "Master" MCU

#include "ev_algo.h"            // Common header for pure Algolion staff
#include "ev_algebra.h"         // The algorithm helpers. Some algebra staff
#include "ev_cyclebuf.h"        // The algorithm helpers. Cycle Buffer staff
#include "ev_calculate.h"       // The algorithm itself
#include "ev_select.h"          // The algorithm helpers. Select Transition Period
#include "ev_security.h"

//#include "environment.h"

#endif /* COMMON_H */
