/*-----------------------------------------------------------------------------
 * Name:		ev_algo.h
 * Purpose:		Common header for pure Algolion staff
 * 				Error processing, additional definitions
 * Revision:	02.00
 * Author:		Anatoly Kh.
 * Copyright:	(C) 2020 Algolion LTD. All rights reserved.
 *----------------------------------------------------------------------------*/
#ifndef EV_ALGO_H
#define EV_ALGO_H

/*
	Global macros
	=============
	defined (or not) in the "Configure Flash Tools..."
	See the tab C/C++ (AC6)
*/
#define	EV_DEBUG		1	// implement debug staff in general code (file ev_algo.c)
#define EV_CALC_DEBUG	1	// implement debug staff for calculations (algorithm implementation) only
#define EV_CPU_ID_MATCH 1	// Check for CPU IDs

/*
	Types definitions
	=================

	All types, that is used in this file, are defined here
*/

#ifndef ULONGLONG
typedef long long ULONGLONG;	// ULONGLONG type
#endif

#ifndef LONG
typedef long LONG;	// LONG type
#endif

#ifndef ULONG
typedef unsigned long ULONG, *PULONG;	// ULONG type
#endif

#ifndef SHORT
typedef short SHORT;	// SHORT type
#endif

#ifndef USHORT
typedef unsigned short USHORT;	// USHORT type
#endif

#ifndef CHAR
typedef char CHAR;	// CHAR type
#endif

#ifndef BYTE
typedef unsigned char BYTE, *PBYTE;	// BYTE type
#endif

#ifndef BOOL
typedef unsigned char BOOL;	// BOOL type
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#define ABS(x)		((x) < 0 ? -(x) : x)
#define MAX(x,y)	((x) < (y) ? (y) : (x))
#define MIN(x,y)	((x) < (y) ? (x) : (y))
#define UNUSED(a)	((void) (a))

#define EV_INLINE inline __attribute__((always_inline)) static

/*
	All allowable Error Codes, 1st parameter of the function "ev_error_handler(errorCode, NULL, 0)"
*/
#define EV_ERROR_HANDLER					 0UL // Came from "Error_Handler()" and from "assert_failed()"
#define EV_ERROR_USART_BUFFER_UNCOMPLETE	 1UL // Number of samples in the buffer too few
#define EV_ERROR_USART_BUFFER_BOTH_ACCUM	 2UL // Both buffers is used for accumulate data
#define EV_ERROR_USART_BUFFER_ILLEGAL		 3UL // Not legal "countSamples"
#define EV_ERROR_USART_RESULTS_ILLEGAL		 4UL // Not legal EV_TP_RESULTS
#define EV_ERROR_USART_DMA_TX0_FAILED		 5UL // USART DMA TX (Raw Data) failed
#define EV_ERROR_USART_DMA_TX1_FAILED		 6UL // USART DMA TX (Results) failed
#define EV_ERROR_USART_DMA_RX_FAILED		 7UL // USART DMA RX failed
#define EV_ERROR_USART_CPLT_TX_FAILED		 8UL // USART DMA Completion Program (TX) failed
#define EV_ERROR_USART_CPLT_RX_FAILED		 9UL // USART DMA Completion Program (RX) failed
#define EV_ERROR_USART_LENGTH				10UL // USART DMA bad length and index
#define EV_ERROR_NO_RAW_DATA_BUFFER			11UL // USART cannot get a buffer (EV_RAW_DATA)
#define EV_ERROR_RAW_DATA_INVALID			12UL // Input data is not valid
#define EV_ERROR_NO_TP_ENTRY				13UL // "ev_select": cannot get a buffer (EV_TP_ENTRY)
#define EV_ERROR_SELECT_INVALID_COUNT		14UL // "ev_select": wrong "EV_TP_ENTRY.tpcount"
#define EV_ERROR_NO_TP_RESULTS				15UL // "ev_select": no buffer "EV_TP_RESULTS"
#define EV_ERROR_CALC_WRONG_INPUT			16UL // during calculation: wrong input parameters in some sub-routine
#define EV_ERROR_CALC_WRONG_LR				17UL // during calculation: wrong sub-routine "Linear Regression"
#define EV_ERROR_ENTRY_TO_SMALL				18UL // Struct EV_TP_ENTRY has no space
#define EV_ERROR_ENTRY_IS_ILLEGAL			19UL // Struct EV_TP_ENTRY has no space
#define EV_ERROR_CAN_DATA					20UL // Illegal CAN data
#define EV_ERROR_APPROXIMATION_FAILED		21UL // General approximation failed
#define EV_ERROR_SELECTION_WRONG_INPUT		22UL // during selection: wrong input parameters in some sub-routine

#define EV_ERROR_MODULE_INITIALIZE			23UL // Wrong sequence of the calls
#define EV_ERROR_EXPIRED_CODE				24UL // Above the "Expiration Day"
#define EV_ERROR_MCU_ID						25UL // Wrong MCU unique ID

#define EV_ERROR_NO_SOC						26UL // No SOC value for the TP
#define EV_ERROR_QOC_R2_GREATER_V			27UL // Group QOC: R2 for voltage is greater than limit (Options.tpDetQOCR2)
#define EV_ERROR_QOC_R2_GREATER_I			28UL // Group QOC: R2 for current is greater than limit (Options.tpDetQOCR2)
#define EV_ERROR_SLOUP_R2_GREATER			29UL // Group Slope: R2 is greater than limit (Options.tpDetSlopeR2)
#define EV_ERROR_TAIL_R2_GREATER_V			30UL // Group Tail: R2 for voltage is greater than limit (Options.tpDetTailR2)
#define EV_ERROR_TAIL_R2_GREATER_I			31UL // Group Tail: R2 for current is greater than limit (Options.tpDetTailR2)
#define EV_ERROR_UNCLASSIFIED_POINT			32UL // Cannot classify the sample (point)

#define EV_ERROR_JUMP_DD_POINTS				33UL // Two transition points in different direction
#define EV_ERROR_TOO_FEW_POINTS				34UL // Too few point if the Transition Period
#define EV_ERROR_POINT_AD					35UL // No Point "A" or "D" in the Transition Period
#define EV_ERROR_POINT_AD_COUNT				36UL // No Counted "A" or "D" in the Transition Period
#define EV_ERROR_TOO_FEW_AD_DIFF			37UL // Between points A and D, current difference < 15A
#define EV_ERROR_POINTS_AD_NOT_OC			38UL // Points A or D current are not OC (<5)
#define EV_ERROR_APPROXIMATION_I3			39UL // Approximation (I3) failed
#define EV_ERROR_APPROXIMATION_I2			40UL // Approximation (I2) failed
#define EV_ERROR_APPROXIMATION_V3			41UL // Approximation (V3) failed
#define EV_ERROR_APPROXIMATION_V2			42UL // Approximation (V2) failed
#define EV_ERROR_NO_CURRENT_JUMP			43UL // No current jump
#define EV_ERROR_TIME_BREAK					44UL // Time Break
#define EV_ERROR_SLOPE_IN_JUMP				45UL // Slope before Jump
#define EV_ERROR_SLOPE_IN_TAIL				46UL // Slope before Jump
#define EV_ERROR_APART_IS_ZERO				47UL // Slope before Jump
#define EV_ERROR_JUMPS_BREAK				48UL // Slope before Jump
#define EV_ERROR_VOLTAGE_ILLEGAL			49UL // Slope before Jump
#define EV_ERROR_CURRENT_ILLEGAL			50UL // Slope before Jump
#define EV_ERROR_TAIL_IN_SLOPE				51UL // Tail point in Slope period
#define EV_ERROR_JUMP_IN_TAIL				52UL // Jump points in Tail period
#define EV_ERROR_SLOPE_DD_POINTS			53UL // Two Slope points in different direction
#define EV_ERROR_CANNOT_ALIGN				54UL // Cannot align Jump points
#define EV_ERROR_NO_BASELINE				55UL // No baseline

#define EV_ERROR_NIDAQ_NOT_INSTALLED		56UL // National Instrument \"Device Monitor\" is not installed on this computer
#define EV_ERROR_NIDAQ_INCOMPATIBLE			57UL // Incompatible version of the National Instrument \"Device Monitor\" is installed

#define EV_ERROR_COUNT						58

#define EV_ERROR_TEXTS	{\
"",\
/*01*/"Number of samples in the buffer too few",\
/*02*/"Both buffers is used for accumulate data",\
/*03*/"Not legal countSamples",\
/*04*/"Not legal EV_TP_RESULTS",\
/*05*/"USART DMA TX (Raw Data) failed",\
/*06*/"USART DMA TX (Results) failed",\
/*07*/"USART DMA RX failed",\
/*08*/"USART DMA Completion Program (TX) failed",\
/*09*/"USART cannot get a buffer (EV_RAW_DATA)",\
/*10*/"USART DMA bad length and index",\
/*11*/"USART cannot get a buffer (EV_RAW_DATA)",\
/*12*/"Input data is not valid",\
/*13*/"ev_select: cannot get a buffer (EV_TP_ENTRY)",\
/*14*/"ev_select: wrong EV_TP_ENTRY.tpcount",\
/*15*/"ev_select: no buffer EV_TP_RESULTS",\
/*16*/"During calculation: wrong input parameters in some sub-routine",\
/*17*/"During calculation: wrong sub-routine Linear Regression",\
/*18*/"Struct EV_TP_ENTRY has no space",\
/*19*/"Struct EV_TP_ENTRY has no space",\
/*20*/"Illegal CAN data",\
/*21*/"General approximation failed",\
/*22*/"During selection: wrong input parameters in some sub-routine",\
/*23*/"Wrong sequence of the calls",\
/*24*/"Above the Expiration Day",\
/*25*/"Wrong MCU unique ID",\
/*26*/"No SOC value for the TP",\
/*27*/"Group QOC: R2 for voltage is greater than limit (Options.tpDetQOCR2)",\
/*28*/"Group QOC: R2 for current is greater than limit (Options.tpDetQOCR2)",\
/*29*/"Group Slope: R2 is greater than limit (Options.tpDetSlopeR2)",\
/*30*/"Group Tail: R2 for voltage is greater than limit (Options.tpDetTailR2)",\
/*31*/"Group Tail: R2 for current is greater than limit (Options.tpDetTailR2)",\
/*32*/"Cannot classify the sample (point)",\
/*33*/"Two transition points in different direction",\
/*34*/"Too few point if the Transition Period",\
/*35*/"No Point 'A' or 'D' in the Transition Period",\
/*36*/"No Counted 'A' or 'D' in the Transition Period",\
/*37*/"Between points A and D, current difference < 15A",\
/*38*/"Points A or D current are not OC (< 5A)",\
/*39*/"Approximation (I3) failed",\
/*40*/"Approximation (I2) failed",\
/*41*/"Approximation (V3) failed",\
/*42*/"Approximation (V2) failed",\
/*43*/"No current jump",\
/*44*/"Time Break",\
/*45*/"Slope in Jump",\
/*46*/"Slope in Tail",\
/*47*/"Counter of a part is zero",\
/*48*/"Too big difference between voltage and current jumps",\
/*49*/"Voltage value is out-of-range",\
/*50*/"Current value is out-of-range",\
/*51*/"Tail point in Slope period",\
/*52*/"Jump points in Tail period",\
/*53*/"Two Slope points in different direction",\
/*54*/"Cannot align Jump points",\
/*55*/"No baseline",\
/*56*/"National Instrument \"Device Monitor\" is not installed on this computer",\
/*57*/"Incompatible version of the National Instrument \"Device Monitor\" is installed",\
};

#define EV_ERROR_SECURITY_TEXTS	{\
/*00*/"",\
/*01*/"Application AlgoShield is expiring soon.",\
/*02*/"Application AlgoShield is expired.",\
/*03*/"No one Security USB Key provided by Algolion.",\
/*04*/"Not legal EV_TP_RESULTS",\
};
#define EV_ERROR_SECURITY_COUNT		5

#define EV_ERRTYPE_SECURED		0	// Security, see EV_ERROR_*** constants
#define EV_ERRTYPE_WIN32		1	// Win32 errors provided by function '::GetLastError()'
#define EV_ERRTYPE_WIN32_HR		2	// Win32 errors provided by function '::GetLastError()'
#define EV_ERRTYPE_ALGO			3	// Errors generated by AlgoShield itself, see EV_ERROR_*** constants
#define EV_ERRTYPE_NI_DAQ		4	// Errors generated by NI DAQ device
#define EV_ERRTYPE_COUNT		5	// How many values like EV_ERRTYPE_***

#define ERROR_CODE_SUCCESS			0 // Parameters' calculation has been successfully completed
#define ERROR_CODE_EXPIRING			1 // Application is expiring soon
#define ERROR_CODE_EXPIRED			2 // Application is expired
#define ERROR_CODE_NOUSBKEY			3 // No the USB Key
#define ERROR_CODE_UNIMLEMENTED		4 // Not implemented



// Configuration of the pack of cell
#define EV_PACK_CELLS_INSUPER		20	// Number of cells in Super-Cell (in parallel)	= 20
#define EV_PACK_SUPERS_INMODULE		12	// Number of Super-Cell in Module (in series)	= 12
#define EV_PACK_MODULES_INPACK		16	// Number of Module in Entire Pack (in series)	= 16
#define EV_PACK_FREQUENCY			100	// Samples per second

/*
	Global structures
	=================
*/

/*
	Configuration Parameters (Settings)
*/
typedef struct EV_CONFIG
{
	// General options (9-24)
	ULONG	goAveSamplingPeriod;	// Average Sampling Period, microseconds
	ULONG	goCellCapacity;			// Nominal capacity (of a single cell), in milliAmpere-hours
	ULONG	goSOC;					// SOC (State-Of-Charge) at the beginning of the file, in tens of percent
	ULONG	goTemperature;			// Temperature at the beginning of the file, in m°C
	float	goZeroDivision;			// Minimal absolute value to deal with
	BYTE	goSamplingMultiplier;   // Default multiplier to calculate 1) maxSamplingTime = goAveSamplingPeriod * goSamplingMultiplier; 2) minSamplingTime = goAveSamplingPeriod / goSamplingMultiplier
	BYTE	goSecuringStatus;		// The software was expired (any except 0x5B and 0x59); The software is expiring soon (0x5B); Else (0x59)
	BYTE	goCustomerID;			// Customer's internal index (see class Customer)
	BOOL	goNIDAQ;				// Collect data (samples) using NI DAQ device

	// Baseline options (8-20)
	float	blStateNormalRng;		// The multiplier of Base Level values to get the range for 'No Fault Found' state-of-safety
	float	blStateFaultRng;		// The multiplier of Base Level values to get the range for 'Fault Found' state-of-safety
	ULONG	blResolutionSOC;		// SOC (State-Of-Charge) resolution while a Baseline lookup, in tens of percent
	ULONG	blResolutionTemp;		// Temperature resolution while a Baseline lookup, in m°C
	BOOL	blBaseline;				// The calculated results should be saved as a "Baseline"
	BOOL	blCompareSameSoc;       // Either select for comparison: only a Baselines with "same" SOC (considering "blResolutionSOC")
	BOOL	blCompareSameTemp;      // Either select for comparison: only a Baselines with "same" Temperature (considering "blResolutionTemp")
	BOOL	blCompareSamePack;      // Either select for comparison: only a Baselines with "same" Cell Pack (see "nameCellPack")

	// Process flags (7-7); Does not save in the file
	BOOL	flagUploadRawdata;		// Either upload Raw Data
	BOOL	flagUploadResults;		// Either upload Results
	BOOL	flagDoCalculation;		// Either do calculation
	BOOL	flagSimulateRawdata;	// Either Simulate Raw Data using the static constants in "environment.c"
	BOOL	flagWasInitialized;		// Either the module was initialized
	BOOL	flagShouldBeTerminated;	// Either the module should be terminated
	BOOL	flagInterfaceProvided;	// Either the [LionSMART] interface was provided

	// Averaging Points "A" and "D" (3-8)
	USHORT	offPointDRight;			// Offset from point "J" to point "D" (offset to the beginning of the interval that to be averaged to get point "D")
	USHORT	offPointALeft;			// Offset from point "A" to point "J" (offset to the beginning of the interval that to be averaged to get point "A")
	BYTE	offPointCount;			// Number of points needed for averaging
	BYTE	reserved2;				// Aligning: 1 byte
	USHORT	offFlags;				// Flags (filters), see FLAG_STRONG_*** constants

	// Definition of bounds values in voltage-current-temperature (VIT) measurement (7-28)
	ULONG	maxSamplingTime;		// Maximum sampling time (time period between 2 sequential samples), microseconds
	ULONG	minSamplingTime;		// Minimum sampling time (time period between 2 sequential samples), microseconds
	ULONG	maxVoltage;				// Maximum voltage (per cell), deci- milli- Volt or 100micro- Volt or 1E-4 * Volt
	ULONG	minVoltage;             // Minimum voltage (per cell), deci- milli- Volt or 100micro- Volt or 1E-4 * Volt
	ULONG	maxCurrent;             // Maximum electric current (per cell), default is 1C current, 1E-4 * Amperes
	LONG	maxTemperature;			// Maximum temperature, in m°C; °C = °K - 273.15, mK (milli-Kelvin)
	LONG	minTemperature;         // Minimum temperature, in m°C; °C = °K - 273.15, mK (milli-Kelvin)

	// Topology of cells pack (4-16)
	ULONG	tplParNum;				// Number of cells in parallel
	ULONG	tplSerNum;				// Number of cells in series
	ULONG	tplPar1st;				// Either parallel-series or series- parallel (Type is ULONG due to aligning)
	ULONG	tplMaxNum;				// Maximal number of cells in a pack

	// Calculations Thresholds (16-64)
	ULONG	tpDetCntMax;			// Maximal number of samples in a Transition Period
	float	tpDetJumpDerV;			// Group 'Jump' samples have voltage derivative more than this, Volts/second
	float	tpDetJumpDerI;			// Group 'Jump' samples have electric current derivative more than this, Amperes/second
	float	tpDetQCCDerV;			// Maximal derivative by voltage of samples in a Group QCC, Volts/second
	float	tpDetQCCDerI;			// Maximal derivative by current of samples in a Group QCC, Amperes/second
	float	tpDetQCCR2;				// Minimal Coefficient of determination of Group QCC during averaging; if the value is zero, do not calculate R2
	ULONG	tpDetQCCNum;			// Maximal number of samples in a Group QCC
	float	tpDetSlopeDerV;			// Minimal derivative by voltage of samples in a Group Slope, Volt/second
	float	tpDetSlopeDerI;			// Minimal derivative by current of samples in a Group Slope, Amperes/second
	float	tpDetSlopeR2;           // Minimal Coefficient of determination of Group Slope during linear regression; if the value is zero, do not calculate R2
	ULONG	tpDetSlopeNum;			// Minimal number of samples in a Group Slope
	float	tpDetTailDerV;			// Maximal derivative by voltage of samples in a Group Tail, Volt/second
	float	tpDetTailDerI;			// Maximal derivative by current of samples in a Group Tail, Amperes/second
	float	tpDetTailR2;            // Minimal coefficient of determination of Group Tail during a linear approximation; if the value is zero, do not calculate R2
	ULONG	tpDetTailNum;           // Minimal number of samples in a Group Tail
	ULONG	reserved3;              // Aligning: 4 bytes

	float	sosThreshold[8];

	BYTE		flagStrongTPType	; // One of Point 'A' and Point 'D' should be Open Circuit (absolute value of current is close to zero) and another should have absolute value of current at least 15A
	BYTE		flagStrongALeft		; // Either 'Slope Point' is not available in the period from point 'D' to Group QCC, or is available
	BYTE		flagStrongDRight	; // Either 'Slope Point' is not available in the period from Group Tail to point 'D', or is available
	BYTE		flagStrongJump		; // Either 'Jump Period'  cannot contains 2 Jump  points in different directions or can
	BYTE		flagStrongSlope		; // Either 'Slope Period' cannot contains 2 Slope points in different directions or can
	BYTE		flagStrongJumpSlope	; // Either 'Jump Period'  cannot contains a Slope point or can
	BYTE		flagStrongSlopeTail	; // Either 'Slope Period' cannot contains a Tail or QCC points or can
	BYTE		flagStrongQCC		; // Either 'QCC Period'   cannot contains a Slope point or can
	BYTE		flagSlopeExists		; // Either 'Slope Period' should exist or not mandatory
	BYTE		flagSlopeAligned	; // Either first points of 'Slope Period' for voltage and current should be aligned or not
	BYTE		flagStrongTail		; // Either 'Tail Period' cannot contains a Slope or a Jump points or can. Is used to calculate the groups.
}
EV_CONFIG, *PEV_CONFIG;
extern EV_CONFIG c;


// Values for PAlgoOptions.offFlags
#define EV_FLAG_STRONG_TP_TYPE		0x01 // One of Points 'A' or 'D' should have absolute value of current is close to zero. If TRUE, a TP is disqualified.
#define EV_FLAG_STRONG_A_LEFT		0x02 // Either 'Slope Point' is not available in the period from point 'A' to Group QCC, or is available. Is used to calculate the groups.
#define EV_FLAG_STRONG_D_RIGHT		0x04 // Either 'Slope Point' is not available in the period from Group Tail to point 'D', or is available. Is used to calculate the groups.
#define EV_FLAG_STRONG_JUMP			0x08 // Either 'Jump Period' cannot contains 2 Jump  points in different directions or can. If TRUE, a TP is disqualified.
#define EV_FLAG_STRONG_SLOPE		0x10 // Either 'Slope Period' cannot contains 2 Slope points in different directions or can. If TRUE, a TP is disqualified.
#define EV_FLAG_STRONG_JUMP_SLOPE	0x20 // Either 'Jump Period' cannot contains a Slope point or can. Is used to calculate the groups.
#define EV_FLAG_STRONG_SLOPE_TAIL	0x40 // Either 'Slope Period' cannot contains a Tail or QCC points or can. Is used to calculate the groups.
#define EV_FLAG_STRONG_QCC			0x80 // Either Either 'QCC Period' cannot contains a Slope point or can. Is used to calculate the groups.
#define EV_FLAG_SLOPE_EXIST			0x100 // Either 'Slope Period' should exist or not mandatory. If TRUE, a TP is disqualified.
#define EV_FLAG_ALIGN_SLOPES		0x200 // Either first points of 'Slope Period' for voltage and current should be aligned or not; only in case that 1st points for voltage and current is aligned. If TRUE, a TP is disqualified.
#define EV_FLAG_STRONG_TAIL			0x400 // Either 'Tail Period' cannot contains a Slope or a Jump points or can. Is used to calculate the groups.


typedef	struct EV_RAW_DATA_SAMPLE_tag
{
	ULONGLONG	rs_time;		// Time, microseconds (relative to the beginning)
	LONG  		current;		// Current, 1E-4 * Ampere;
	ULONG 		voltagea[EV_PACK_SUPERS_INMODULE];		// Voltage, 1E-4 * Volt;
	BYTE   		ctypeV;			// Transition Flags - voltage, see "EV_STYPE_***" in "framework.h"
	BYTE   		ctypeI;			// Transition Flags - current, see "EV_STYPE_***" in "framework.h"
	BYTE	  	r2V;			// reserved
	BYTE	  	r2I;			// reserved
	ULONG		reserved1;		// reserved
}
EV_RAW_DATA_SAMPLE, *PEV_RAW_DATA_SAMPLE;

// Transition Flags for the fields "ctypeI" and "ctypeV" - Types of a sample
#define EV_STYPE_UNKNOWN			0x00
#define EV_STYPE_JUMP				0x03
#define EV_STYPE_JUMP_DROP			0x01
#define EV_STYPE_JUMP_RAISE			0x02
#define EV_STYPE_SLOPE				0x0C
#define EV_STYPE_SLOPE_DROP			0x04
#define EV_STYPE_SLOPE_RAISE		0x08
#define EV_STYPE_TAIL				0x10
#define EV_STYPE_QCC				0x20
#define EV_STYPE_TIME_BREAK			0x80


#define	EV_TP_SAMPLE_COUNT			30		// Number of samples in struct AlgoTPEntry (see "AlgoOptions.cs", line 514)
typedef	struct AlgoTPEntry_tag
{
	// Next member are set during "TP Selection"
	BYTE		count_tp				; // Number of samples in a Transition Period
	BYTE		count_qcc				; // Number of "QOC", counting by Voltage
	BYTE		count_qccI				; // Number of "QOC", counting by Current
	BYTE		count_jump				; // Number of "Jump", counting by Voltage
	BYTE		count_jumpI				; // Number of "Jump", counting by Current
	BYTE		count_slope				; // Number of "Slope", counting by Voltage
	BYTE		count_slopeI			; // Number of "Slope", counting by Current
	BYTE		count_tail				; // Number of "Tail", counting by Voltage
	BYTE		count_tailI				; // Number of "Tail", counting by Current
	BYTE		type_tp					; // TP type, see EV_TYPE_TP_***
	BYTE		is_completed			; // This TP is completed
	BYTE		is_baseline				; // This TP results is Base Line

	BYTE		rankCountPFF			; // Count parameters, that are ranked as "PFF"
	BYTE		rankCountFF				; // Count parameters, that are ranked as "FF"
	short		index_tp				; // Index of this TP within a file; starts with 0
	ULONGLONG	time_tp					; // Start time; time of the first sample; array "samples" contains relative times

	// Intermediate parameters
	double		SOC						; // State-Of-Charge at time "time_tp" in range [0, 1]
	double		qccV					; // Voltage for the Group QCC (average)
	double		qccI					; // Current for the Group QCC (average)
	double		qccR2					; // Coefficient of determination of Group QCC during averaging
	double		slopeK					; // dV / dt for the Group Slope
	double		slopeV					; // Voltage for the Group Slope
	double		pointBV					; // Point "B" voltage
	double		pointBI					; // Point "B" current
	double		tailV					; // Average voltage for the Group Tail
	double		tailI					; // Average current for the Group Tail

	double			coef		[12]	; // Coefficients of Approximation: "A", "B", "C" and "t0" for 3-param approximation; "A" and "C" for 2-param approximation;  twice - for current and voltage
	double			param		[8]		; // Results
	BYTE			paramMask	[8]		; // 0 - in the range; 1 - out of the "Good" range; 2 - out of the "Dangerous" range
	EV_RAW_DATA_SAMPLE	samples	[EV_TP_SAMPLE_COUNT]; // Array of samples (of points) of the Transition Period
}
AlgoTPEntry, *PAlgoTPEntry;
typedef	const AlgoTPEntry *PCAlgoTPEntry;

// Possible TP types, see the field "type_tp", see C# for m_typeTPStrings = { "Unknown", "OC - Discharge", "OC - Charge", "Charge - OC", "Discharge - OC" };
#define EV_TYPE_TP_UNKNOWN			0x00	// Unknown
#define EV_TYPE_TP_OC_DROP			0x01	// OC -> drop	== "OC - Discharge"
#define EV_TYPE_TP_OC_RAISE			0x02	// OC -> raise	== "OC - Charge"
#define EV_TYPE_TP_DROP_OC			0x03	// drop  -> OC	== "Charge - OC"
#define EV_TYPE_TP_RAISE_OC			0x04	// raise -> OC	== "Discharge - OC"


#define EV_CYCLE_BUF_SIZE			0x80 // =128. Size (number of elements) of the Cycle Buffer. Should be equal to 2**N
typedef struct EV_GLOBALS_tag
{
	// Temporary. The following are constant during a calculation session.
	ULONG		maxVoltage_fp		; // Maximum voltage (full pack!), 1E-4 * Volt; = Options.maxVoltage * tplSerNum
	ULONG		minVoltage_fp		; // Minimum voltage (full pack!), 1E-4 * Volt; = Options.minVoltage * tplSerNum
	ULONG		maxCurrent_fp		; // Maximum electric current (full pack!), 1E-4 * Amperes;
	LONG		offsetStart			; // Maximal TP length; Start offset to the beginning of TP
	ULONG		offsetSkipCheck		; // Numbers of sample to skip (to prevent process same TP and wait for first possible TP)
	ULONG		maxCurrent0			; // All current values that is less, treated as "Open Circuit" (0.1C current)

	LONG		errorCode;			// Error Code or Completion Code (zero means all OK)
	SHORT		countTP;			// Number of TPs successfully processed

	ULONG		uDerivativeCalc		; // Global counter of calculated derivatives dI/dt;
	float		fDerivativeMaxI		; // Global maximum of calculated derivatives dI/dt;
	float		fDerivativeMaxV		; // Global maximum of calculated derivatives dV/dt;

	// State-Of_Charge
	double		capacity			; // Calculated (as I*dt) capacity, in Ampere * hours
	double		capacityMax			; // Max value of capacity
	double		capacityMin			; // Min value of capacity

	// Index within the voltage array
	unsigned char	iv				; // Index within the voltage array

	// Objects
	AlgoTPEntry		Entry			; // Intermediate and final results

	ULONGLONG	uTimeHRateLimit		; // Limit (of 'uCountSample') to save the raw data log with high DAQ rate
	ULONG		uCountSample		; // Global counter of valid samples; Index within the file

#ifdef EV_DEBUG // To find the CURRENT SAMPLE:
	ULONG		dbgSampleIndex		; // Index within the input file
	ULONG		dbgSampleIndexCheck	; // -"-, of the sample to test flags
	ULONG		dbgIndexCBuffer		; // Index within the cycle buffer
	ULONG		dbgIndexCBufferCheck; // -"-, of the sample to test flags
	ULONGLONG	dbgSampleTime		; // Time
	ULONGLONG	dbgSampleTimeCheck	; // -"-, of the sample to test flags
#endif

	EV_RAW_DATA_SAMPLE	samplesAcc	; // Samples' accumulator to average
	ULONG			samplesAccCount	; // Number of samples accumulated in 'samplesAcc'

	ULONG			errorIndex		; // Index within a 'EV_ERROR_ITEM' array

	// Array of the last samples
	EV_RAW_DATA_SAMPLE	cb_samp			[EV_CYCLE_BUF_SIZE]; // Type of a sample, see EV_STYPE_***; the current index is [(BYTE) uCountSample]
}
EV_GLOBALS, *PEV_GLOBALS;
extern EV_GLOBALS g;
typedef const EV_GLOBALS PCEV_GLOBALS;


/*
	Structure that receives results of Averaging
*/
typedef struct EV_AVERAGE_tag
{
	double	avg_x;	// Average of the x members
	double	sum_x2;	// Sum of X * X
	double	r2;		// Calculated R squared of the average
}
EV_AVERAGE, *PEV_AVERAGE;

/*
	Structure that receives results of Linear Regression
	F(t) = A * t + B; where A (=slope) and B (=value0) are unknowns.
*/
typedef struct EV_LINEAR_REGR_tag
{
	double	avg_x;		// Average of the x members
	double	avg_y;		// Average of the y members
	double	slope;		// the calculated slope		of the regression line; see coefficient A above
	double	value0;		// the calculated intercept of the regression line and axis Y; see coefficient B above
	double	r2;			// the calculated R squared of the regression line
}
EV_LINEAR_REGR, *PEV_LINEAR_REGR;

/*
	Structure that receives results of Approximation
	F(t) = A * [ 1 / [t - t0] **2 ] + B * [ 1 / [t - t0] ] + C; where A, B, C and t0 are unknowns.
*/
typedef struct EV_APPROX_tag
{
	double	coefA;		// coefficient A - see above
	double	coefB;		// coefficient B - see above
	double	coefC;		// coefficient C - see above
	double	t0;			// shift axis "t" (zero point)
	double	r2;			// the calculated R squared of the approximation curve
}
EV_APPROX, *PEV_APPROX;


#define	EV_TP_PARAMS_COUNT			5	// Number of resulted parameters


/**
  *	Results of approximation and calculation one single Transition Period
  * - time and set of the parameters
  */
typedef struct EV_TP_RESULTS_tag
{
	ULONGLONG		time_tp; 	// Start time of the TP
	short			index_tp;	// Index of the TP

	// Ranked results as array of values:
	// ALGO_CELL_UNKNOWN or ALGO_CELL_NORMAL or ALGO_CELL_ATTENSION or ALGO_CELL_CRITICAL
	// for each (super-)cell in module
	unsigned char	tp_rank[EV_PACK_SUPERS_INMODULE];
}
EV_TP_RESULTS, *PEV_TP_RESULTS;
typedef const EV_TP_RESULTS *PCEV_TP_RESULTS;

extern EV_TP_RESULTS	gResults;	// Results, buffer #0


#define	EV_TP_HISTORY_COUNT			10	// Number of kept results
#define	EV_TP_HISTORY_KEEP_FIRST	3	// Number of first results which are used as "Based Line"

/**
  * The history of results, saved in FLASH on the SLAVE
  * Keep in the struct:
  * 		1) First EV_TP_HISTORY_KEEP_FIRST (3) measurements, thats are avereging and used as "Based Line"
  *			2) Last [EV_TP_HISTORY_COUNT - EV_TP_HISTORY_KEEP_FIRST] (7) measurement.
  *			   When new measurement occurs these are shifted to receive one more.
  * Size of EV_TP_RES_ONE is 64 bytes
  * Size: 64 * (10 * 12) + 8 = 7,688;
  * Pages (256 bytes): 30 full page + 8 bytes
  **/
typedef struct EV_TP_HISTORY_tag
{
	unsigned char	hi_count;		// Number of filled members "tp_res"
	unsigned char	magic[7];		// Bytes are used to detect first call (if no match, means un-initialized)
	//EV_TP_RES_ONE	tp_res[EV_TP_HISTORY_COUNT][EV_PACK_SUPERS_INMODULE]; // Results
}
EV_TP_HISTORY, *PEV_TP_HISTORY;

extern EV_TP_HISTORY	gHistory;	// The history of results


/*
	The stack of last 100 errors
*/
#define EV_ERROR_STACK_COUNT	100
#define EV_ERROR_SOURCE_LENGTH	26	// To align = 8 * n + 2
#define EV_ERROR_TEXT_LENGTH	280	// To align = 8 * n


typedef struct EV_ERROR_ITEM_tag
{
	LONG		code;		// Error Code
	BYTE		type;		// Type of the error code, see EV_ERRTYPE_*** constants
	BYTE		severity;	// Flag (TRUE or FALSE). If "TRUE", stop the calculation
	CHAR		source[EV_ERROR_SOURCE_LENGTH];	// Where it appeared - file and line number
	ULONGLONG	time;		// When it appeared - Timestamp from the sample
	CHAR		text[EV_ERROR_TEXT_LENGTH];		// Text connected with this error
}
EV_ERROR_ITEM, *PEV_ERROR_ITEM;
extern EV_ERROR_ITEM e[EV_ERROR_STACK_COUNT];

#define EV_ERROR1(code)							ev_error_handler(code, EV_ERRTYPE_ALGO, FALSE, __FILE__, __LINE__, 0   , NULL)
#define EV_ERROR2(code, time)					ev_error_handler(code, EV_ERRTYPE_ALGO, FALSE, __FILE__, __LINE__, time, NULL)
//#define EV_ERROR3(code)							ev_error_handler(code, EV_ERRTYPE_ALGO, TRUE , __FILE__, __LINE__, 0   , NULL)
#define EV_ERROR3(code)							assert(code == 0)
#define EV_ERROR5(code, type, sev, time, text)	ev_error_handler(code, type           , sev  , __FILE__, __LINE__, time, text)
#define EV_ASSERT(code, condition)				if (!(condition)) { EV_ERROR3(code); assert(condition); }

#define EV_ERRORA								ev_error_handler(::GetLastError(), EV_ERRTYPE_WIN32, TRUE , __FILE__, __LINE__, 0, NULL)
#define EV_ERRORA1								ev_error_handler(::GetLastError(), EV_ERRTYPE_WIN32, FALSE, __FILE__, __LINE__, 0, NULL)
#define EV_ERRORB(code)							ev_error_handler(code, EV_ERRTYPE_SECURED, TRUE, __FILE__, __LINE__, 0, NULL)
#define EV_ERRORC(code, text)					ev_error_handler(code, EV_ERRTYPE_NI_DAQ , TRUE, __FILE__, __LINE__, 0, text)


void ev_error_handler(const LONG _code, const BYTE _type, const BYTE _severity,
	const char* _source, const LONG _line, const ULONGLONG _time, const CHAR* _text);


EV_INLINE
double ev_sqrt(double r)
{
	return sqrt(r);
}


EV_INLINE
void ev_zeromem(void* buffer, long length)
{
	memset(buffer, 0, length);
}

EV_INLINE
void ev_copymem(void* destination, const void* source, long length)
{
	memcpy(destination, source, length);
}


EV_INLINE
PAlgoTPEntry ev_get_actual_entry()
{
	return &g.Entry;
}


/*
	Get real time - number of microseconds from the beginning of 1970 UTC.
	Read RTC registers and calculate real time.
	Source: "stm32F413.pdf"
*/
#define RTC_TR_VALUE   		*((PULONG) 0x40002800UL) // Time register value
#define RTC_DR_VALUE   		*((PULONG) 0x40002804UL) // Date register value
#define RTC_PREP_VALUE 		*((PULONG) 0x40002810UL) // RTC prescaler register value
#define RTC_SSR_VALUE  		*((PULONG) 0x40002828UL) // RTC sub-second register value
#define RTC_TR_MASK   		0x007F7F7FUL // Time register value mask to get 0x00HHMMSS (hours-minutes-seconds) in BCD format
#define RTC_DR_MASK   		0x00FF1F3FUL // Date register value mask to get 0x00YYMMDD (year-month-day) in BCD format
#define RTC_PREP_MASK		0x00007FFFUL // RTC prescaler register value mask to get 0x0000SSSS (PREDIV_S) in hex format
#define RTC_SSR_MASK		0x0000FFFFUL // RTC sub-second value mask to get 0x0000XXXX, second fraction = (PREDIV_S - X) / (PREDIV_S + 1)
#define BCD_DIGIT(x,n)		((x >> (n)) & 0x0F) // x - value, n - number of skipped bits from the left
#define BCD2_TO_HEX(x,n)	(BCD_DIGIT(x,n+4) * 10 + BCD_DIGIT(x,n)) // 2-digit BCD value to hex
EV_INLINE
ULONGLONG ev_timer()
{
	ULONG rtc_prep = RTC_PREP_VALUE & RTC_PREP_MASK; // RTC prescaler register
	ULONG rtc_ssr  = RTC_SSR_VALUE  & RTC_SSR_MASK; // RTC sub-second register
	time_t result = time(NULL);

	if (rtc_ssr >= rtc_prep) result++;
	result = result * 1000000ULL + (rtc_prep - rtc_ssr) * 1000000ULL / (rtc_prep + 1);
	return result;
}


// Slave #1, Prashant-provided
#define DEVICE_ID_N0_R0 		0x003A003D	// Last 2 digits: 0011 1101
#define DEVICE_ID_N0_R1 		0x0400C000
#define DEVICE_ID_N0_R2 		0x43E943E9

// Slave #2, Prashant-provided
#define DEVICE_ID_N1_R0 		0x003A004D	// Last 2 digits: 0100 1101
#define DEVICE_ID_N1_R1 		0x0400C000
#define DEVICE_ID_N1_R2 		0x43E943E9

// Slave #3, Prashant-provided
#define DEVICE_ID_N2_R0 		0x003A003F	// Last 2 digits: 0011 1111
#define DEVICE_ID_N2_R1 		0x0400C000
#define DEVICE_ID_N2_R2 		0x43E943E9

// Slave #4, Prashant-provided
#define DEVICE_ID_N3_R0 		0x003A004E	// Last 2 digits: 0100 1110
#define DEVICE_ID_N3_R1			0x0400C000
#define DEVICE_ID_N3_R2			0x43E943E9

// Algolion debug on the development board STM32F413H-DISCO
#define DEVICE_ID_N4_R0 		0x001F0044
#define DEVICE_ID_N4_R1			0x30365111
#define DEVICE_ID_N4_R2 		0x36303638

#define MCU_ID_R0_VALUE   		*((PULONG) 0x1FFF7A10UL) // MCU ID register #0
#define MCU_ID_R1_VALUE   		*((PULONG) 0x1FFF7A14UL) // MCU ID register #1
#define MCU_ID_R2_VALUE   		*((PULONG) 0x1FFF7A18UL) // MCU ID register #2

#define EXPIRATION_DATE 0x00210600 // 2021-06-01

EV_INLINE
BOOL ev_secured()
{
	ULONG rtc_dr, r0;

	// Check expiration
	rtc_dr = RTC_DR_VALUE & RTC_DR_MASK; // RTC Date register
	if (rtc_dr > EXPIRATION_DATE)
		return FALSE;

	if ((MCU_ID_R0_VALUE == DEVICE_ID_N4_R0) && (MCU_ID_R1_VALUE == DEVICE_ID_N4_R1) && (MCU_ID_R2_VALUE == DEVICE_ID_N4_R2))
		return TRUE;

	if ((MCU_ID_R1_VALUE != DEVICE_ID_N0_R1) || (MCU_ID_R2_VALUE != DEVICE_ID_N0_R2))
		return FALSE;

	r0 = MCU_ID_R0_VALUE;
	return (r0 == DEVICE_ID_N0_R0) || (r0 == DEVICE_ID_N1_R0) || (r0 == DEVICE_ID_N2_R0) || (r0 == DEVICE_ID_N3_R0);
}


/**
  *	@brief Initialize global Data Acquisition Configuration
  *		   in the Slave module
  */
EV_INLINE
void ev_init_slave(void)
{
	// EV_CONFIG		c;
	ev_zeromem(&c, sizeof(EV_CONFIG));

	c.goAveSamplingPeriod		= 20000;
	c.goCellCapacity			= 2850;
	c.goSOC						= 1000;
	c.goTemperature				= 20000;
	c.goZeroDivision			= 1.0E-30F;
	c.goSamplingMultiplier		= 3;

	c.blStateNormalRng			= 2.1F;
	c.blStateFaultRng			= 5.5F;
	c.blResolutionSOC			= 100;
	c.blResolutionTemp			= 20000;
	c.blBaseline				= FALSE;
	c.blCompareSameSoc			= TRUE;
	c.blCompareSameTemp			= FALSE;
	c.blCompareSamePack			= FALSE;

	c.flagUploadRawdata			= TRUE;
	c.flagUploadResults			= FALSE;
	c.flagDoCalculation			= TRUE;
	c.flagSimulateRawdata		= FALSE;
	c.flagWasInitialized		= FALSE;
	c.flagShouldBeTerminated	= FALSE;
	c.flagInterfaceProvided		= FALSE;

	c.offPointALeft				= 2;
	c.offPointDRight			= 2;
	c.offPointCount				= 2;
	c.offFlags					= 0;

	c.maxSamplingTime			= 100000;
	c.minSamplingTime			= 20000;
	c.maxVoltage				= 44000;
	c.minVoltage				= 25000;
	c.maxCurrent				= 50000;
	c.maxTemperature			= 120000;
	c.minTemperature			= -50000;

	// Topology of cells pack (4-16)
	c.tplParNum					= 20;
	c.tplSerNum					= 1;
	c.tplPar1st					= TRUE;
	c.tplMaxNum					= 200;

	// Calculations Thresholds (16-64)
	c.tpDetCntMax				= EV_TP_SAMPLE_COUNT - 1;
	c.tpDetJumpDerV				= 1.8F;
	c.tpDetJumpDerI				= 200.0F;
	//		Group "QCC"
	c.tpDetQCCDerV				= 0.18F;
	c.tpDetQCCDerI				= 2.0F;
	c.tpDetQCCR2				= 0.60F;
	c.tpDetQCCNum				= 3;
	//		Group "Slope"
	c.tpDetSlopeDerV			= 0.1F;
	c.tpDetSlopeDerI			= 30.0F;
	c.tpDetSlopeR2				= 0.60F;
	c.tpDetSlopeNum				= 2;
	//		Group "Tail"
	c.tpDetTailDerV				= 0.18F;
	c.tpDetTailDerI				= 2.0F;
	c.tpDetTailR2				= 0.60F;
	c.tpDetTailNum				= 5;

	c.sosThreshold[0]			= 0.015;
	c.sosThreshold[1]			= 0.001;
	c.sosThreshold[2]			= 100;

	c.flagStrongTPType			= FALSE; // One of Point 'A' and Point 'D' should be Open Circuit (absolute value of current is close to zero) and another should have absolute value of current at least 15A
	c.flagStrongALeft			= FALSE; // Either 'Slope Point' is not available in the period from point 'D' to Group QCC, or is available
	c.flagStrongDRight			= FALSE; // Either 'Slope Point' is not available in the period from Group Tail to point 'D', or is available
	c.flagStrongJump			= FALSE; // Either 'Jump Period'  cannot contains 2 Jump  points in different directions or can
	c.flagStrongSlope			= FALSE; // Either 'Slope Period' cannot contains 2 Slope points in different directions or can
	c.flagStrongJumpSlope		= FALSE; // Either 'Jump Period'  cannot contains a Slope point or can
	c.flagStrongSlopeTail		= FALSE; // Either 'Slope Period' cannot contains a Tail or QCC points or can
	c.flagStrongQCC				= FALSE; // Either 'QCC Period'   cannot contains a Slope point or can
	c.flagSlopeExists			= FALSE; // Either 'Slope Period' should exist or not mandatory
	c.flagSlopeAligned			= FALSE; // Either first points of 'Slope Period' for voltage and current should be aligned or not
	c.flagStrongTail			= FALSE; // Either 'Tail Period' cannot contains a Slope or a Jump points or can. Is used to calculate the groups.

	// EV_ERROR_ITEM	e[EV_ERROR_STACK_COUNT];
	ev_zeromem(&e, sizeof(EV_ERROR_ITEM) * EV_ERROR_STACK_COUNT);

	// EV_GLOBALS		g;
	ev_zeromem(&g, sizeof(EV_GLOBALS));

	g.maxVoltage_fp		= c.maxVoltage * c.tplSerNum; // Maximum voltage (full pack!), 1E-4 * Volt; = Options.maxVoltage * tplSerNum
	g.minVoltage_fp		= c.minVoltage * c.tplSerNum; // Minimum voltage (full pack!), 1E-4 * Volt; = Options.minVoltage * tplSerNum
	g.maxCurrent_fp		= c.maxCurrent * c.tplParNum; // Maximum electric current (full pack!), 1E-4 * Amperes;
	g.offsetStart 		= EV_TP_SAMPLE_COUNT - 5; // Maximal TP length; Start offset to the beginning of TP
	g.offsetSkipCheck	= g.offsetStart; // Numbers of sample to skip (to prevent process same TP and wait for first possible TP)
	g.maxCurrent0		= c.goCellCapacity; // MilliAmperes * 10 (to 1E-4) / 10 (0.1C current)

	// [Ampere * hours] = [tens of percent, 1E-3] * [milli-Ampere-hours, 1E+3]
	// Calculated (as I*dt) Capacitance, in Ampere * hours
	g.capacity = 1E-6 * c.goSOC * c.goCellCapacity * c.tplParNum * c.tplSerNum;
	g.capacityMin = +1E+30;
	g.capacityMax = -1E+30;

	// EV_TP_RESULTS	gResults0
	// EV_TP_RESULTS	gResults1
	ev_zeromem(&gResults, sizeof(EV_TP_RESULTS));

	// EV_TP_HISTORY	gHistory;	// The history of results
	ev_zeromem(&gHistory, sizeof(EV_TP_HISTORY));
}


#endif  // EV_ALGO_H
