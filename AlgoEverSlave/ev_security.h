/*-----------------------------------------------------------------------------
 * Name:		ev_security.h
 * Purpose:		Common header to secure Algolion staff
 * Revision:	02.00
 * Author:		Anatoly Kh.
 * Copyright:	(C) 2020 Algolion LTD. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef EV_SECURITY_H
#define EV_SECURITY_H

// Slave #1, Prashant-provided
#define gDeviceID_N0_R0 0x003A003D
#define gDeviceID_N0_R1 0x0400C000
#define gDeviceID_N0_R2 0x43E943E9

// Slave #2, Prashant-provided
#define gDeviceID_N1_R0 0x003A004D
#define gDeviceID_N1_R1 0x0400C000
#define gDeviceID_N1_R2 0x43E943E9

// Slave #3, Prashant-provided
#define gDeviceID_N2_R0 0x003A003F
#define gDeviceID_N2_R1 0x0400C000
#define gDeviceID_N2_R2 0x43E943E9

// Slave #4, Prashant-provided
#define gDeviceID_N3_R0 0x003A004E
#define gDeviceID_N3_R1 0x0400C000
#define gDeviceID_N3_R2 0x43E943E9

// Algolion debug on the development board STM32F413H-DISCO
#define gDeviceID_N4_R0 0x001F0044
#define gDeviceID_N4_R1 0x30365111
#define gDeviceID_N4_R2 0x36303638

// Slave #5, : Shreyas-provided
#define gDeviceID_N5_R0 0x00490030
#define gDeviceID_N5_R1 0x33365109
#define gDeviceID_N5_R2 0x37303838


/******************************************************************************
 * @brief		Return the value of the 1st or 2nd or 3rd MCU ID register
 * @param[in]	i - index of the register
 * @return		Value of the 0x1FFF7A10 or 0x1FFF7A14 or 0x1FFF7A18
 */
//EV_INLINE
unsigned int ev_read_mcu_id(const unsigned char i)
{
	// Register 0x1FFF7A10	= 0001 1111 1111 1111   0111 1010 0001 0000
	//						  0000 0111 1111 1111   1101 1110 1000 0100
	// 0x1FFF7A10 / 4		= 0000 0111 1111 1111   1101 1110 1000 0100
	unsigned int mcuid;
	unsigned int* pr = (unsigned int*) ((0x07FFDE84 + i) << 2);
	mcuid = *pr;
	return mcuid;
}


#define CHECK_MCU(a) if (mcuid == gDeviceID_N##a##_R0) \
		return ((ev_read_mcu_id(1) == gDeviceID_N##a##_R1) && (ev_read_mcu_id(2) == gDeviceID_N##a##_R2));

//EV_INLINE
BOOL ev_check_mcu_id(void)
{
	unsigned int mcuid;
	mcuid = ev_read_mcu_id(0);
	if (mcuid == gDeviceID_N0_R0) return ((ev_read_mcu_id(1) == gDeviceID_N0_R1) && (ev_read_mcu_id(2) == gDeviceID_N0_R2));
	if (mcuid == gDeviceID_N1_R0) return ((ev_read_mcu_id(1) == gDeviceID_N1_R1) && (ev_read_mcu_id(2) == gDeviceID_N1_R2));
	if (mcuid == gDeviceID_N2_R0) return ((ev_read_mcu_id(1) == gDeviceID_N2_R1) && (ev_read_mcu_id(2) == gDeviceID_N2_R2));
	if (mcuid == gDeviceID_N3_R0) return ((ev_read_mcu_id(1) == gDeviceID_N3_R1) && (ev_read_mcu_id(2) == gDeviceID_N3_R2));
	if (mcuid == gDeviceID_N4_R0) return ((ev_read_mcu_id(1) == gDeviceID_N4_R1) && (ev_read_mcu_id(2) == gDeviceID_N4_R2));
	if (mcuid == gDeviceID_N5_R0) return ((ev_read_mcu_id(1) == gDeviceID_N5_R1) && (ev_read_mcu_id(2) == gDeviceID_N5_R2));
	return TRUE;
}

#endif // EV_SECURITY_H

