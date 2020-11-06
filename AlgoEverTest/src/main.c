/**
  ******************************************************************************
  * @file    main.c
  * @author  Anatoly Kh.
  * @version 02.00
  * @date    25-October-2020
  * @brief   Test implementation to check Algolion(C) AlgoShield(R).
  * 		 Program language:	C
  * 		 IDE:				Eclipse, AC6
  * 		 Main function:		main
  * 		 Tested on:			MCU STM32F413ZHTx, EB STM32F413H-DISCO
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "string.h"
#include "time.h"

#define EV_SLAVE 1					// Make "LionSmart_Algolion.h" produces the program interface for Slave MCU
#include "./../../AlgoEverSlave/LionSmart_Algolion.h"	// LIONSmart(C) interface for "Slave" and "Master" MCU

#define EV_TEST_CAN_BUS_ID		11111 // Replace it! // 1st parameter for the function "CAN_TxMsg"; From slave to master
#define EV_TEST_SERIES_COUNT	30 // Number of measurements in the test data

#define EV_SUPERCELL_COUNT		20 // A Supercell count: 20 single cells in parallel
#define EV_MODULE_COUNT			12 // A Module count: 12 supercells in series
#define EV_FULLPACK_COUNT		16 // A Full Pack count: 16 modules in series
								   // Total cells	20 × 12 × 16 = 3840 cells
								   // Frequency of data sampling 20Hz (period is 50mls)

/*
 * Test values of voltage
 *
 * The following array presents 30 (EV_TEST_SERIES_COUNT) samples; each contains 12 (EV_MODULE_COUNT) values.
 * Units is 100uV.
 * Before the Transition Point (sample number 10) should be at least 10 samples (not mandatory same).
 * After the Transition Point should be at least 20 samples (not mandatory same).
 *
 * In other words, it presents 12 (EV_MODULE_COUNT) time-series of voltage for each battery module.
 * First module should produce ALGO_CELL_NORMAL(1) State-of-Safety;
 * second module should produce ALGO_CELL_CRITICAL(3) State-of-Safety;
 * the others modules (from 3 up to 12) should produce ALGO_CELL_UNKNOWN(0).
 */
uint16_t TestVoltageArray[EV_TEST_SERIES_COUNT][EV_MODULE_COUNT] = {
	{ 41120, 41120, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 1st measurement at 50 milliseconds
	{ 41120, 41120, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 2nd measurement at 100 milliseconds
	{ 41120, 41120, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 3
	{ 41120, 41120, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 4
	{ 41120, 41120, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 5
	{ 41120, 41120, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 6
	{ 41120, 41120, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 7
	{ 41120, 41120, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 8
	{ 41120, 41120, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 9  dV/dt = 0
	{ 41120, 41120, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 10 dV/dt = -5.56
	{ 38340, 38720, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 11 dV/dt = -0.62
	{ 38160, 38410, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 12 dV/dt = -0.64
	{ 38020, 38090, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 13 dV/dt = -0.12
	{ 38010, 38030, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 14 dV/dt = -0.04
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 15 dV/dt = 0
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 16
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 17
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 18
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 19
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 20
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 21
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 22
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 23
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 24
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 25
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 26
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 27
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 28
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 },		// 29
	{ 38010, 38010, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000 }		// 30s measurement at 1.5 seconds
};

/*
 * Test values of current
 * The following array presents one time-series of current values.
 * One current value for a battery module (for each supercell that contains 20 cells in parallel).
 * Units is 100uA.
 *
 * !NOTE! It is physically incorrect use same time-series for ALGO_CELL_UNKNOWN (voltage series from 3 up to 12).
 *        But for the test it is OK.
 */
int32_t TestCurrentArray[EV_TEST_SERIES_COUNT] = {
	-18700,		// 1st measurement at 50 milliseconds
	-18700,		// 2nd measurement at 100 milliseconds
	-18700,		// 3
	-18700,		// 4
	-18700,		// 5
	-18700,		// 6
	-18700,		// 7
	-18700,		// 8
	-18700,		// 9  dI/dt = 0
	-18700,		// 10 dI/dt = -205.
	-121560,	// 11 dI/dt = -181.
	-212100,	// 12 dI/dt = -147.5
	-285880,	// 13 dI/dt = 0
	-285880,	// 14
	-285880,	// 15
	-285880,	// 16
	-285880,	// 17
	-285880,	// 18
	-285880,	// 19
	-285880,	// 20
	-285880,	// 21
	-285880,	// 22
	-285880,	// 23
	-285880,	// 24
	-285880,	// 25
	-285880,	// 26
	-285880,	// 27
	-285880,	// 28
	-285880,	// 29
	-285880		// 30s measurement at 1.5 seconds
};

uint16_t TestDataIndex; // Will be used as 0, 1, 2, ... 29 - index of current sample


/**********************************************************************************************
 * LionSmart's functions.
 * It is the !test! implementations
 * Functions
 * 	LMM_GetCellTemperaturePerModule, FM_ALGO_ReadFromFlashMemory, FM_ALGO_WriteToFlashMemory
 * 	have !dummy! implementations
 */
uint32_t  LMM_GetMeasTimeStamp(void)
{
	// Real-Time SysTick implementation:
	//return HAL_GetTick();

	// Test implementation:
	return (TestDataIndex + 1) * 50; // Just 50, 100, 150, ... milliseconds. Time 0 is illegal.
}


uint16_t* LMM_GetCellVoltagesPerModule(void)
{
	return TestVoltageArray[TestDataIndex];  // Return 1-dim. array of voltage values
}


int32_t   LMM_GetCurrentPerModule(void)
{
	return TestCurrentArray[TestDataIndex];  // Return a current value
}

uint16_t* LMM_GetCellTemperaturePerModule(void)
{
	return NULL;  // Not used
}

uint8_t   FM_ALGO_ReadFromFlashMemory(uint32_t address, uint8_t *data, uint32_t dataLength)
{
	return 0;  // Not need NOW - the function is used to restore the previous results between calculation sessions (between "Power OFF" and "Power ON")
}

uint8_t   FM_ALGO_WriteToFlashMemory(uint8_t *data, uint32_t dataLength)
{
	return 0;  // Not need NOW - the function is used to save the results between calculation sessions (between "Power OFF" and "Power ON")
}
/*
 * END of LionSmart functions test implementation
 **********************************************************************************************/


int main(void)
{
	// Reserve a space for results
	uint8_t initialized, Results[EV_MODULE_COUNT];
	const uint8_t *pResults;

	//
	// Step 1. Initialize interface structure
	//
	LMM_INTERFACE_SLAVE Interface = {
		LMM_GetMeasTimeStamp,
		LMM_GetCellVoltagesPerModule,
		LMM_GetCurrentPerModule,
		LMM_GetCellTemperaturePerModule,
		FM_ALGO_ReadFromFlashMemory,
		FM_ALGO_WriteToFlashMemory,
		EV_TEST_CAN_BUS_ID,
		0
	};

	//
	// Step 2. Send the interface structure to Algolion staff
	//
	ALGO_SlaveSetupInterface(&Interface);

	//
	// Step 3. Initialize Algolion staff
	//
	initialized = ALGO_SlaveInitialize();
	if (initialized) // If something wrong
		return initialized;

	//
	// Step 4. Notify Algolion staff about next measurement was done
	//
	// The AlgoSheild accumulates new sample (time, voltages, current) in the Transition Period Buffer
	//
	for(TestDataIndex = 0; TestDataIndex < EV_TEST_SERIES_COUNT; TestDataIndex++)
	{
		ALGO_SlaveEventTrigger(); // Do it EV_TEST_SERIES_COUNT times

		/*	Real-Time SysTick implementation:
			Waiting 50 milliseconds; */
		// HAL_Delay(50);
	}

	//
	// Step 5. Processes the accumulated Transition Period (if any exists), Stop and Release Algolion staff
	//
	// The AlgoSheild processes the accumulated Transition Period in 3 cases:
	//		1) New Transition Period starts
	//		2) Transition Period Buffer is full
	//		3) User called "ALGO_SlaveTerminate" function
	//
	ALGO_SlaveTerminate();

	//
	// Step 6. Get results
	//
	// Function "ALGO_SlaveGetSoS" might be called in any time.
	// But, if no Transition Period processed before,
	// it return "ALGO_CELL_UNKNOWN" values
	//
	pResults = ALGO_SlaveGetSoS();
	memcpy(Results, pResults, EV_MODULE_COUNT); // Save results locally
	return 0;
}

