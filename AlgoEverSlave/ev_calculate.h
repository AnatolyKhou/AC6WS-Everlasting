/*-----------------------------------------------------------------------------
 * Name:		ev_calculate.h
 * Purpose:		The algorithm itself.
 * Revision:	02.00
 * Author:		Anatoly Kh.
 * Copyright:	(C) 2020 Algolion LTD. All rights reserved.
 *----------------------------------------------------------------------------*/
#ifndef EV_CALCULATE_H
#define EV_CALCULATE_H

/**
  * @brief	Calculate ALGO parameters.
  * @param[in,out]	pEntry	- pointer to the Heating Period
  */
EV_INLINE
BOOL ev_algo_params(PAlgoTPEntry pEntry)
{
	EV_LINEAR_REGR	slopeLR;				// Return value from linear approximation
	BYTE			iStart, iEnd, i, drop;

#ifndef VERSION_ENGINEER
	if (!ev_secured()) return FALSE;
#endif // VERSION_ENGINEER

	//EV_ASSERT(EV_ERROR_NO_SOC, (pEntry->SOC >= 0) && (pEntry->SOC <= 1.0F));
	// Calculate SOC at this TP like defined above, see formula (1).
	// Only one value for SOC might be taken, because of TP duration is 5 – 6 seconds.
	// SOC is the first parameter P1 or BL1
	// SOC is calculated during reading Raw Data - it should be exist

	// Calculate linear approximation of voltage curve of Group Slope
	iStart	= pEntry->count_qcc + pEntry->count_jump;
	iEnd	= iStart + pEntry->count_slope + 1;
	drop = (pEntry->type_tp == EV_TYPE_TP_OC_DROP) || (pEntry->type_tp == EV_TYPE_TP_DROP_OC);

	if (ev_liregr_v(pEntry, iStart, iEnd, &slopeLR   ) &&
		// Slope has same sign as drop
		(((slopeLR.slope <= -c.goZeroDivision) &&  drop) ||
		 ((slopeLR.slope >=  c.goZeroDivision) && !drop)))
	{
		pEntry->slopeK = slopeLR.slope;
		pEntry->slopeV = (float) slopeLR.value0;
		pEntry->pointBV = slopeLR.slope * pEntry->coef[3] + slopeLR.value0;

		if (slopeLR.r2 >= c.tpDetSlopeR2)
			// Capacitance: Capacitance (I_qoc - I_tail) / K_slope
			pEntry->param[2] = (pEntry->tailI - pEntry->qccI) / pEntry->slopeK * 1E-6;
	}
	else
	{
		for (i = iStart; i < iEnd; i++)
			pEntry->slopeV += pEntry->samples[i].voltagea[g.iv];
		pEntry->slopeK	= 0;
		pEntry->slopeV /= (iEnd - iStart);
		pEntry->pointBV	= pEntry->slopeV;
	}

	// Calculate resistance (P3 or BL3) R_ir = (V_qoc - V_slope) / (I_qoc - I_tail)
	pEntry->param[0] = (pEntry->qccV - pEntry->pointBV) / (pEntry->qccI - pEntry->tailI);

	// Calculate resistance (P4 or BL4) R_rx = (V_slope - V_tail) / (I_qoc-I_tail)
	pEntry->param[1] = (pEntry->pointBV - pEntry->tailV) / (pEntry->qccI - pEntry->tailI);

	return TRUE;
}


#define MIN_R2 0.10

/**
  *	@brief	Solution. Calculate all parameters
  *
  *	@param[in] pEntry		- selected TP, input
  *
  * @return Either succeeded or not
  */
EV_INLINE
BOOL ev_calc_res(PAlgoTPEntry pEntry)
{
	EV_APPROX approx;
	BYTE iEnd, iStart, minDuration;
	BOOL rc;

#ifndef VERSION_ENGINEER
	if (!ev_secured()) return FALSE;
#endif // VERSION_ENGINEER

	minDuration = 6;

	//
	// Approximate Voltage using 3 parameters:
	//
	iStart = (BYTE) (pEntry->count_qcc + 1);
	iEnd   = (BYTE) MAX(pEntry->count_qcc + pEntry->count_jump + pEntry->count_slope, iStart + minDuration);
	assert(iEnd - iStart >= minDuration);
	assert( pEntry->samples[iStart - 1].ctypeV & EV_STYPE_JUMP);
	assert((pEntry->samples[iStart - 2].ctypeV & EV_STYPE_JUMP) == 0);

	rc = ev_approximate(pEntry, iStart, iEnd, TRUE, TRUE, &approx);
	if (rc && (approx.r2 >= MIN_R2))
	{
		assert((approx.t0 >= pEntry->samples[iStart - 1].rs_time) && (approx.t0 <= pEntry->samples[iStart].rs_time));
		pEntry->coef[0] = approx.coefA;
		pEntry->coef[1] = approx.coefB;
		pEntry->coef[2] = approx.coefC;
		pEntry->coef[3] = approx.t0;
	}
	else
	{
		EV_ERROR1(EV_ERROR_APPROXIMATION_V3);
		pEntry->coef[3] = (double) pEntry->samples[pEntry->count_qcc + 1].rs_time;
	}

	//
	// Approximate Voltage using 2 parameters:
	//
	rc = ev_approx(pEntry, iStart, iEnd, TRUE, FALSE, pEntry->coef[3], &approx);
	if (rc && (approx.r2 >= MIN_R2))
	{
		pEntry->coef[4] = approx.coefA;
		pEntry->coef[5] = approx.coefC;
		pEntry->param[3] = ABS(approx.coefA) * 1.0E-12;
	}
	else
	{
		EV_ERROR1(EV_ERROR_APPROXIMATION_V2);
	}

	if (pEntry->count_jumpI > 0)
	{
		//
		// Approximate Current using 3 parameters
		//
		iStart = (BYTE) (pEntry->count_qccI + 1);
		iEnd   = (BYTE) MAX(pEntry->count_qccI + pEntry->count_jumpI + pEntry->count_slopeI, iStart + minDuration);

		assert(iEnd - iStart >= minDuration);
		assert( pEntry->samples[iStart - 1].ctypeI & EV_STYPE_JUMP);
		assert((pEntry->samples[iStart - 2].ctypeI & EV_STYPE_JUMP) == 0);

		// Approximate Current using 3 parameters:
		rc = ev_approximate(pEntry, iStart, iEnd, FALSE, TRUE, &approx);
		if (rc && (approx.r2 >= MIN_R2))
		{
			assert((approx.t0 >= pEntry->samples[iStart - 1].rs_time) && (approx.t0 <= pEntry->samples[iStart].rs_time));
			pEntry->coef[6] = approx.coefA;
			pEntry->coef[7] = approx.coefB;
			pEntry->coef[8] = approx.coefC;
			pEntry->coef[9] = approx.t0;
		}
		else
		{
			EV_ERROR1(EV_ERROR_APPROXIMATION_I3);
			pEntry->coef[9] = (double) pEntry->samples[pEntry->count_qccI + 1].rs_time;
		}

		//
		// Approximate Current using 2 parameters:
		//
		rc = ev_approx(pEntry, iStart, iEnd, FALSE, FALSE, pEntry->coef[9], &approx);
		if (rc && (approx.r2 >= MIN_R2))
		{
			pEntry->coef[10] = approx.coefA;
			pEntry->coef[11] = approx.coefC;
			pEntry->param[4] = ABS(approx.coefA) * 1.0E-15;
		}
		else
		{
			EV_ERROR1(EV_ERROR_APPROXIMATION_I2);
		}
	}

	rc = ev_algo_params(pEntry);
	return rc;
}


#endif // EV_CALCULATE_H
