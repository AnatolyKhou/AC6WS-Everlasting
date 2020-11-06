/*-----------------------------------------------------------------------------
 * Name:		ev_select.h
 * Purpose:		The algorithm helpers. Select Transition Period.
 * Revision:	02.00
 * Author:		Anatoly Kh.
 * Copyright:	(C) 2020 Algolion LTD. All rights reserved.
 *----------------------------------------------------------------------------*/
#ifndef EV_SELECT_H
#define EV_SELECT_H

#define EV_LOG_BEFORE_JUMP		20
#define EV_LOG_AFTER_JUMP		200
#define EV_LOG_DAQ_RATE_DECR	100

// Forward declarations:
EV_INLINE void ev_select_tp();

/******************************************************************************
 * @brief			Check the values of sample - Time, Voltage and Current
 * @param[in]		sp_time		-
 * @param[in]		sp_voltage	-
 * @param[in]		sp_current	-
 * @return			TRUE if all OK; FALSE else
 */
EV_INLINE BOOL
ev_select_check(const ULONGLONG sp_time_last, PEV_RAW_DATA_SAMPLE raw)
{
	if ((ULONG) ABS(raw->current) > g.maxCurrent_fp)
	{
		EV_ERROR2(EV_ERROR_CURRENT_ILLEGAL, raw->rs_time);
		return FALSE;
	}
	for (BYTE k = 0; k < EV_PACK_SUPERS_INMODULE; k++)
	{
		if ((raw->voltagea[k] > g.maxVoltage_fp) || (raw->voltagea[k] < g.minVoltage_fp))
		{
			EV_ERROR2(EV_ERROR_VOLTAGE_ILLEGAL, raw->rs_time);
			return FALSE;
		}
	}
	if ((sp_time_last + c.minSamplingTime > raw->rs_time) ||
		(sp_time_last + c.maxSamplingTime < raw->rs_time))
	{
		EV_ERROR2(EV_ERROR_TIME_BREAK, raw->rs_time);
		return FALSE;
	}
	return TRUE;
}


/******************************************************************************
 * @brief		Put new sample (point) to Cycle Buffer and set the
 *				Current and Voltage Flags
 * @param[in]	pSample - Address of a raw sample
 */
EV_INLINE
void ev_select_sample_flags(PEV_RAW_DATA_SAMPLE pSample)
{
	PEV_RAW_DATA_SAMPLE pCurr, pLast;
	float derivI, derivIA, derivV, derivVA;

	pCurr  = g.cb_samp + ( g.uCountSample      & EV_CYCLE_BUF_MASK);
	pLast  = g.cb_samp + ((g.uCountSample - 1) & EV_CYCLE_BUF_MASK);
	ev_copymem(pCurr, pSample, sizeof(EV_RAW_DATA_SAMPLE));

    //if (pSample->rs_time == 7440000000ULL)
	//	DebugBreak();
	//if (g.uCountSample == 1648)
	//	DebugBreak();

    if ((pLast->ctypeV & EV_STYPE_TIME_BREAK) == 0)
    {
        derivI = ev_cb_derivI(pLast, pCurr);
        derivIA = ABS(derivI);
        derivV = ev_cb_derivV(pLast, pCurr);
        derivVA = ABS(derivV);

        if (g.fDerivativeMaxI < derivIA)
            g.fDerivativeMaxI = derivIA;
        if (g.fDerivativeMaxV < derivVA)
            g.fDerivativeMaxV = derivVA;

        // Current Flags of a sample, see EV_STYPE_***
		if (derivIA >= c.tpDetJumpDerI)
            pLast->ctypeI |= (derivI < 0) ? EV_STYPE_JUMP_DROP : EV_STYPE_JUMP_RAISE;
		else {
			if (derivIA >= c.tpDetSlopeDerI)
				pLast->ctypeI |= (derivI < 0) ? EV_STYPE_SLOPE_DROP : EV_STYPE_SLOPE_RAISE;
			if (derivIA < c.tpDetQCCDerI)
				pLast->ctypeI |= EV_STYPE_QCC;
			if (derivIA < c.tpDetTailDerI)
				pLast->ctypeI |= EV_STYPE_TAIL;
		}

        // Voltage Flags of a sample, see EV_STYPE_***
		if (derivVA >= c.tpDetJumpDerV)
            pLast->ctypeV |= (derivV < 0) ? EV_STYPE_JUMP_DROP : EV_STYPE_JUMP_RAISE;
		else {
			if (derivVA >= c.tpDetSlopeDerV)
				pLast->ctypeV |= (derivV < 0) ? EV_STYPE_SLOPE_DROP : EV_STYPE_SLOPE_RAISE;
			if (derivVA < c.tpDetQCCDerV)
				pLast->ctypeV |= EV_STYPE_QCC;
			if (derivVA < c.tpDetTailDerV)
				pLast->ctypeV |= EV_STYPE_TAIL;
		}
    }
}


EV_INLINE BOOL
ev_select_counters(PAlgoTPEntry pEntry)
{
	PEV_RAW_DATA_SAMPLE pSamp;
	short offset, offsetFrom, offsetTo;
	BYTE type, typeJump = 0, typeFirst = 0;
	BYTE countI, delta;

	offsetFrom = -(short) (c.tpDetQCCNum);
	offsetTo = (short) c.tpDetCntMax - (short) c.tpDetQCCNum;

	//
	// Voltage counters:
	//
	for (offset = offsetFrom; offset < offsetTo; offset++)
	{
		pSamp = ev_cb_get_samp(offset);
		type = pSamp->ctypeV;

		// Check: if exist EV_STYPE_TIME_BREAK in the TP?
		if (((type | pSamp->ctypeI) & EV_STYPE_TIME_BREAK) != 0)
		{
			EV_ERROR1(EV_ERROR_TIME_BREAK);
			return FALSE;
		}

		typeJump = type & EV_STYPE_JUMP;
		// Is the point Jump?
		if (typeJump != 0)
		{
			// YES, the point is Jump
			if (pEntry->count_jump == 0)
				typeFirst = typeJump;
			else if (c.flagStrongJump && (typeFirst != typeJump))
			{
				EV_ERROR1(EV_ERROR_JUMP_DD_POINTS);
				return FALSE;
			}
			// There were Tail points?
			if (pEntry->count_tail > 0)
			{
				if (c.flagStrongTail)
				{
					EV_ERROR1(EV_ERROR_JUMP_IN_TAIL);
					return FALSE;
				}
				else
				{
					pEntry->count_jump--;
					pEntry->count_tail++;
				}
			}
			// There were Slope points?
			if (pEntry->count_slope > 0)
			{
				if (c.flagStrongJumpSlope)
				{
					EV_ERROR1(EV_ERROR_SLOPE_IN_JUMP);
					return FALSE;
				}
				pEntry->count_jump += pEntry->count_slope;
				pEntry->count_slope = 0;
			}
			pEntry->count_jump++;
		}

		// The point is not Jump.
		// There were Jump point?
		else if (pEntry->count_jump == 0)
		{
			// No, There were not Jump point
			pEntry->count_qcc++;
		}

		// The point is not Jump.
		// There were Jump points.
		// Is the point Slope?
		else if ((type & EV_STYPE_SLOPE) != 0)
		{
			// Yes. The point is Slope.
			if (c.flagStrongSlope &&
				(((type & EV_STYPE_SLOPE_DROP ) && (typeJump & EV_STYPE_JUMP_RAISE)) ||
				((type & EV_STYPE_SLOPE_RAISE) && (typeJump & EV_STYPE_JUMP_DROP ))))
			{
				EV_ERROR1(EV_ERROR_SLOPE_DD_POINTS);
				return FALSE;
			}
			if ((c.flagStrongSlopeTail) || (pEntry->count_tail == 0))
			{
				pEntry->count_slope += pEntry->count_tail;
				pEntry->count_tail = 0;
				pEntry->count_slope++;
			}
			else
			{
				if ((c.flagStrongDRight) && (pEntry->count_tail > 0))
				{
					EV_ERROR1(EV_ERROR_SLOPE_IN_TAIL);
					return FALSE;
				}
				pEntry->count_tail++;
			}
		}
		else
		{
			if (pEntry->count_tail >= c.tpDetTailNum)
				break;
			pEntry->count_tail++;
		}
		pEntry->count_tp++;
	}
	assert(pEntry->count_tp == pEntry->count_qcc + pEntry->count_jump + pEntry->count_slope + pEntry->count_tail);
	assert(pEntry->count_tp <= c.tpDetCntMax);

	//
	// Current counters:
	//
	for (offset = offsetFrom; offset < offsetTo; offset++)
	{
		pSamp = ev_cb_get_samp(offset);
		type = pSamp->ctypeI;

		typeJump = type & EV_STYPE_JUMP;
		// Is the point Jump?
		if (typeJump != 0)
		{
			// YES, the point is Jump
			if (pEntry->count_jumpI == 0)
				typeFirst = typeJump;
			else if (c.flagStrongJump && (typeFirst != typeJump))
			{
				EV_ERROR1(EV_ERROR_JUMP_DD_POINTS);
				return FALSE;
			}
			// There were Tail points?
			if (pEntry->count_tailI > 0)
			{
				if (c.flagStrongTail)
				{
					EV_ERROR1(EV_ERROR_JUMP_IN_TAIL);
					return FALSE;
				}
				else
				{
					pEntry->count_jumpI--;
					pEntry->count_tailI++;
				}
			}
			// There were Slope points?
			if (pEntry->count_slopeI > 0)
			{
				if (c.flagStrongJumpSlope)
				{
					EV_ERROR1(EV_ERROR_SLOPE_IN_JUMP);
					return FALSE;
				}
				pEntry->count_jumpI += pEntry->count_slopeI;
				pEntry->count_slopeI = 0;
			}
			pEntry->count_jumpI++;
		}

		// The point is not Jump.
		// There were Jump point?
		else if (pEntry->count_jumpI == 0)
		{
			// No, There were not Jump point
			pEntry->count_qccI++;
		}

		// The point is not Jump.
		// There were Jump points.
		// Is the point Slope?
		else if ((type & EV_STYPE_SLOPE) != 0)
		{
			// Yes. The point is Slope.
			if (c.flagStrongSlope &&
				(((type & EV_STYPE_SLOPE_DROP ) && (typeJump & EV_STYPE_JUMP_RAISE)) ||
				 ((type & EV_STYPE_SLOPE_RAISE) && (typeJump & EV_STYPE_JUMP_DROP ))))
			{
				EV_ERROR1(EV_ERROR_SLOPE_DD_POINTS);
				return FALSE;
			}
			if ((c.flagStrongSlopeTail) || (pEntry->count_tailI == 0))
			{
				pEntry->count_slopeI += pEntry->count_tailI;
				pEntry->count_tailI = 0;
				pEntry->count_slopeI++;
			}
			else
			{
				if ((c.flagStrongDRight) && (pEntry->count_tailI > 0))
				{
					EV_ERROR1(EV_ERROR_SLOPE_IN_TAIL);
					return FALSE;
				}
				pEntry->count_tailI++;
			}
		}
		else
		{
			if (pEntry->count_tailI >= c.tpDetTailNum)
				break;
			pEntry->count_tailI++;
		}
	}

	// No Jump points?
	if ((pEntry->count_jump == 0) || (pEntry->count_jumpI == 0))
		return FALSE;
	if ((c.goCustomerID == 6 /*NYSERDA*/) && ((pEntry->count_tail == 0) || (pEntry->count_tailI == 0)))
		return FALSE;

	// Aligning current and voltage counters
	countI = pEntry->count_qccI + pEntry->count_jumpI + pEntry->count_slopeI + pEntry->count_tailI;
	if (pEntry->count_tp > countI)
		pEntry->count_tailI += pEntry->count_tp - countI;
	else if (pEntry->count_tp < countI)
	{
		pEntry->count_tail += countI - pEntry->count_tp;
		pEntry->count_tp = countI;
	}

	if (c.flagSlopeExists && (pEntry->count_slope == 0))
	{
		EV_ERROR1(EV_ERROR_APART_IS_ZERO);
		return FALSE;
	}
	/*
	if (MAX(pEntry->count_qcc, pEntry->count_qccI) - MIN(pEntry->count_qcc, pEntry->count_qccI) >= 4)
	{
		EV_ERROR_IND(EV_ERROR_JUMPS_BREAK, 0);
		return FALSE;
	}
	if ((pEntry->count_qcc == 0) || (pEntry->count_jump == 0) || (pEntry->count_slope < c.tpDetSlopeNum))
	{
		EV_ERROR_IND(EV_ERROR_APART_IS_ZERO, 0);
		//return FALSE;
	}
	*/

	if (c.flagSlopeAligned && (ABS(pEntry->count_qcc - pEntry->count_qccI) <= 1) &&
		(pEntry->count_qcc + pEntry->count_jump < pEntry->count_qccI + pEntry->count_jumpI))
	{
		delta = (BYTE) (pEntry->count_qccI + pEntry->count_jumpI - pEntry->count_qcc - pEntry->count_jump);
		if (pEntry->count_slope < delta)
		{
			EV_ERROR1(EV_ERROR_CANNOT_ALIGN);
			return FALSE;
		}
		pEntry->count_jump += delta;
		pEntry->count_tail -= delta;
	}
	return TRUE;
}


/******************************************************************************
 * @brief		Combine a sample (time-voltage-current); check it;
 *				and accumulate samples in the Cycle Buffer
 * @param[in]	sp_time		- time in microseconds
 * @param[in]	sp_voltage	- voltage in 0.0001 Volts
 * @param[in]	sp_current	- current in 0.0001 Amperes
 */
EV_INLINE void
ev_select(PEV_RAW_DATA_SAMPLE raw)
{
	PEV_RAW_DATA_SAMPLE pLastSamp;

	// Last call?
	if (raw->rs_time == 0)
	{
		for (; g.offsetStart > 15; g.offsetStart --)
			ev_select_tp();
		return;
	}

	pLastSamp = ev_cb_get_last();

#ifdef EV_DEBUG // To find the CURRENT SAMPLE:
	g.dbgSampleIndex		= g.uCountSample								; // Index within the input file
	g.dbgSampleIndexCheck	= g.uCountSample - g.offsetStart				; // -"-, of the sample to test flags
	g.dbgIndexCBuffer		= g.uCountSample & EV_CYCLE_BUF_MASK			; // Index within the cycle buffer
	g.dbgIndexCBufferCheck	= g.dbgSampleIndexCheck & EV_CYCLE_BUF_MASK		; // -"-, of the sample to test flags
	g.dbgSampleTime			= (g.cb_samp + g.dbgIndexCBuffer)->rs_time		; // Time
	g.dbgSampleTimeCheck	= (g.cb_samp + g.dbgIndexCBufferCheck)->rs_time	; // -"-, of the sample to test flags

	//if (g.dbgSampleIndexCheck == 16087)
	//	DebugBreak();
#endif // EV_DEBUG

	// First call?
	if (g.uCountSample == 0)
	{
		// No time: 1st sample
		ev_select_check(0, raw);
		ev_copymem(g.cb_samp, raw, sizeof(EV_RAW_DATA_SAMPLE));
		g.uCountSample = 1;
		return;
	}

	ev_select_check(pLastSamp->rs_time, raw);
	ev_select_sample_flags(raw);

	g.uCountSample++;

	// SOC
	// Ampere * seconds = [1E+6 * seconds, 1E-6] * [1E+4 * Ampere, 1E-4]
	g.capacity += /* 1E-10 / 3600 */ 2.777778E-14
		* (raw->rs_time - pLastSamp->rs_time) * ((pLastSamp->current + raw->current) >> 1);

	g.capacityMax = MAX(g.capacityMax, g.capacity);
	g.capacityMin = MIN(g.capacityMin, g.capacity);

	if (g.offsetSkipCheck > 0)
	{
		g.offsetSkipCheck--;
		return;
	}

	ev_select_tp();
}


/******************************************************************************
 * @brief			Finalize the Transition Period
 *					and call to calculate the parameters
 * @param[in,out]	pEntry - Transition Period object
 */
EV_INLINE
void ev_select_tp()
{
	PAlgoTPEntry pEntry = NULL;
	PEV_RAW_DATA_SAMPLE pSample;
	ULONG pointAcurrent, pointDcurrent;
	short offset;
	BYTE type, typeStrong, calcOK, rank;

	pSample = ev_cb_get_samp(0);
	type = pSample->ctypeV | pSample->ctypeI;

	// 1st point should be "Jump" (Point "J")
	if ((type & EV_STYPE_JUMP) == 0)
		return;

	//if (pSample->rs_time == 3097411000LL)
	//	assert(FALSE);

	// QCC period check
	typeStrong = c.flagStrongQCC ? EV_STYPE_TIME_BREAK | EV_STYPE_JUMP | EV_STYPE_SLOPE : EV_STYPE_TIME_BREAK | EV_STYPE_JUMP;
	for (offset = -1; offset >= -(short) c.tpDetQCCNum; offset--)
	{
		pSample = ev_cb_get_samp(offset);
		type = pSample->ctypeV | pSample->ctypeI;
		if ((type & typeStrong) != 0)
			goto move_index_and_return;
	}

#ifndef VERSION_ENGINEER
	if (!ev_secured()) return;
#endif // VERSION_ENGINEER

	pEntry = ev_get_actual_entry();
	ev_zeromem(pEntry, sizeof(AlgoTPEntry));

	pEntry->is_completed = (BYTE) ev_select_counters(pEntry);

	assert(pEntry->count_tp <= EV_TP_SAMPLE_COUNT);

	// Completed?
	if (pEntry->is_completed)
	{
		// Final conclusion
		gResults.index_tp = pEntry->index_tp;
		gResults.time_tp  = pEntry->time_tp;
		memset(gResults.tp_rank, ALGO_CELL_UNKNOWN, EV_PACK_SUPERS_INMODULE);

		for (g.iv = 0; g.iv < EV_PACK_SUPERS_INMODULE; g.iv ++)
		{

			// Point "A" and point "D" exist?
			if (!ev_cb_get_pointA(pEntry) ||
				!ev_cb_get_pointD(pEntry))
			{
				EV_ERROR1(EV_ERROR_POINT_AD);
				continue;
			}

			pEntry->is_baseline = c.blBaseline;
			pEntry->SOC = g.capacity / (1.0E-3 * c.goCellCapacity * c.tplParNum * c.tplSerNum);

			// "Strong TP Types" means the beginning of the TP is OC or the ending is OC
			if (c.flagStrongTPType && ((ABS(pEntry->qccI) > g.maxCurrent0) && (ABS(pEntry->tailI) > g.maxCurrent0)))
			{
				EV_ERROR1(EV_ERROR_POINTS_AD_NOT_OC);
				continue;
			}

			// Rounding:
			pointAcurrent = (ULONG) (ABS(pEntry->qccI) + 0.5);
			pointDcurrent = (ULONG) (ABS(pEntry->tailI) + 0.5);

			if (pEntry->qccV > pEntry->tailV)
				pEntry->type_tp = (pointAcurrent < pointDcurrent) ? EV_TYPE_TP_OC_DROP : EV_TYPE_TP_DROP_OC;
			else
				pEntry->type_tp = (pointAcurrent < pointDcurrent) ? EV_TYPE_TP_OC_RAISE : EV_TYPE_TP_RAISE_OC;

			ev_cb_copy_samples(pEntry);

			calcOK = ev_calc_res(pEntry);
			if (calcOK)
			{
				pEntry->index_tp = g.countTP++;

				rank = ALGO_CELL_UNKNOWN;
				for (BYTE i = 0; i < sizeof(c.sosThreshold) / sizeof(c.sosThreshold[0]); i++)
				{
					if (c.sosThreshold[i] > 0)
					{
						if ((c.sosThreshold[i] * 2 >= pEntry->param[i]) && (c.sosThreshold[i] / 2 <= pEntry->param[i]))
							rank = MAX(rank, ALGO_CELL_NORMAL);
						else if ((c.sosThreshold[i] * 2 >= pEntry->param[i]) && (c.sosThreshold[i] / 2 <= pEntry->param[i]))
							rank = MAX(rank, ALGO_CELL_ATTENSION);
						else
							rank = MAX(rank, ALGO_CELL_CRITICAL);
					}
				}
				gResults.tp_rank[g.iv] = rank;
			}
		}
	}

	move_index_and_return:
	g.offsetSkipCheck ++;
	if (pEntry != NULL)
		g.offsetSkipCheck += MAX(pEntry->count_qcc + pEntry->count_jump, pEntry->count_qccI + pEntry->count_jumpI) - c.tpDetQCCNum;
}

#endif // EV_SELECT_H
