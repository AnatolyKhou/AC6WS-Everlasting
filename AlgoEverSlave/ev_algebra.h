/*-----------------------------------------------------------------------------
 * Name:		ev_algebra.h
 * Purpose:		The algorithm helpers. Some algebra staff.
 * Revision:	02.00
 * Author:		Anatoly Kh.
 * Copyright:	(C) 2020 Algolion LTD. All rights reserved.
 *----------------------------------------------------------------------------*/
#ifndef EV_ALGEBRA_H
#define EV_ALGEBRA_H


/**
	Solve the equations:
		G0 * x + G1 * y = G2
		G3 * x + G4 * y = G5
	Method: Cross the lines
		D =  G0 * G4 + G3 * G1
		X = (G2 * G4 + G5 * G1) / D
		Y = (G0 * G5 + G3 * G2) / D

	@param[in]		G[6] - Coefficients of the equations
	@param[in,out]	R[2] - Roots of the equations
	@return 		success (ABS(D) != 0)
*/
EV_INLINE
BOOL ev_solve_2x2(double E[6], double R[2])
{
	double determinant = E[0] * E[4] - E[3] * E[1];
	if (ABS(determinant) < c.goZeroDivision)
		return FALSE;
	R[0] = (E[2] * E[4] - E[5] * E[1]) / determinant;
	R[1] = (E[0] * E[5] - E[3] * E[2]) / determinant;
	return TRUE;
}


/**
	Solve the equations:
		E00 * x + E01 * y + E02 * z = E03		x = E03/E00 - E01/E00 * y - E02/E00 * z
		E04 * x + E05 * y + E06 * z = E07		(E05 - E04*E01/E00) * y + (E06 - E04*E02/E00) * z = E07 - E04*E03/E00
		E08 * x + E09 * y + E10 * z = E11		(E09 - E08*E01/E00) * y + (E10 - E08*E02/E00) * z = E11 - E08*E03/E00
	Method: Substitute

	@param[in]		E[12] - Coefficients of the equations
	@param[in,out]	R[3]  - Roots of the equations
	@return success
*/
EV_INLINE
BOOL ev_solve_3x3(double E[12], double R[3])
{
	double i, j, k;
	double G[6];
	R[0] = R[1] = R[2] = 0;

	if (ABS(E[0]) < c.goZeroDivision)
		return FALSE;

	i = E[1] / E[0];
	j = E[2] / E[0];
	k = E[3] / E[0];

	G[0] = E[ 5] - E[4] * i;
	G[1] = E[ 6] - E[4] * j;
	G[2] = E[ 7] - E[4] * k;
	G[3] = E[ 9] - E[8] * i;
	G[4] = E[10] - E[8] * j;
	G[5] = E[11] - E[8] * k;

	if (ev_solve_2x2(G, &R[1]))
	{
		R[0] =  (E[3] - E[1] * R[1] - E[2] * R[2]) / E[0];
		return TRUE;
	}
	return FALSE;
}


#ifdef EV_CALC_DEBUG
/**
	Solve the equations:
		E00 * x + E01 * y + E02 * z = E03
		E04 * x + E05 * y + E06 * z = E07
		E08 * x + E09 * y + E10 * z = E11
	Method: Cross of the planes (determinant)

	@param[in]		E[12] - Coefficients of the equations
	@param[in,out]	R[3]  - Roots of the equations
	@return success
*/
EV_INLINE
BOOL ev_solve_3x3_A(double E[12], double R[3])
{
	double determinant;
	double D00, D01, D02, D10, D11, D12, D20, D21, D22;

	D00 = E[ 5] * E[10] - E[ 9] * E[ 6];
	D01 = E[ 1] * E[10] - E[ 9] * E[ 2];
	D02 = E[ 1] * E[ 6] - E[ 5] * E[ 2];

	D10 = E[ 4] * E[10] - E[ 8] * E[ 6];
	D11 = E[ 0] * E[10] - E[ 8] * E[ 2];
	D12 = E[ 0] * E[ 6] - E[ 4] * E[ 2];

	D20 = E[ 4] * E[ 9] - E[ 8] * E[ 5];
	D21 = E[ 0] * E[ 9] - E[ 8] * E[ 1];
	D22 = E[ 0] * E[ 5] - E[ 4] * E[ 1];

	determinant = E[0] * D00 - E[1] * D10 + E[2] * D20;
	if (ABS(determinant) < c.goZeroDivision)
		return FALSE;

	R[0] = ( E[ 3] * D00 - E[ 7] * D01 + E[11] * D02) / determinant;
	R[1] = (-E[ 3] * D10 + E[ 7] * D11 - E[11] * D12) / determinant;
	R[2] = ( E[ 3] * D20 - E[ 7] * D21 + E[11] * D22) / determinant;
	return TRUE;
}
#endif // EV_CALC_DEBUG


/**
  * @brief	Solve (find the root) of a quadratic equation, like
  *			Y = A / x**2 + B / x + C, where y, A, B, C is given real numbers
  *
  * @param[in]      Y	 - the TP object
  * @param[in]      R[3] - the coefficients A, B and C as array
  * @return			The root (biggest one)
  */
EV_INLINE
double ev_solve_quadratic(const double Y, const double R[3])
{
	if (ABS(R[0]) < c.goZeroDivision) return 0;
	double C = R[2] - Y;
	double det = R[1] * R[1] - 4.0 * R[0] * C;
	if (det < 0) return 0;

	double detsr = ev_sqrt(det);
	double root = (R[0] < 0)
		? (-R[1] - detsr) / 2.0 / R[0]
		: (-R[1] + detsr) / 2.0 / R[0];

	if ((ABS(root) < c.goZeroDivision) || (root < 0))
		return 0;
	 return 1.0 / root;
}

/**
  * @brief	Calculates the voltage derivative value with respect to time
  *
  * @param[in]      pEntry - the TP object
  * @param[in]      i	   - index of a sample to calculate derivative (cannot be last one)
  * @return			The voltage derivative
  */
EV_INLINE
double ev_der_v(PAlgoTPEntry pEntry, const BYTE i)
{
	BYTE i1 = i + 1;
	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, i < pEntry->count_tp - 1);
	return
		((double) pEntry->samples[i].voltagea[g.iv] - pEntry->samples[i1].voltagea[g.iv]) /
		((double) pEntry->samples[i].rs_time - pEntry->samples[i1].rs_time) ;
}


/**
  * @brief	Calculates the current derivative value with respect to time
  *
  * @param[in]		pEntry - the TP object
  * @param[in]		i	   - index of a sample to calculate derivative (cannot be last one)
  * @return			The voltage derivative
  */
EV_INLINE
double ev_der_i(PAlgoTPEntry pEntry, const BYTE i)
{
	BYTE i1 = i + 1;
	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, i < pEntry->count_tp - 1);
	return (double)
		((double) pEntry->samples[i].current - pEntry->samples[i1].current) /
		((double) pEntry->samples[i].rs_time - pEntry->samples[i1].rs_time) ;
}


/**
  *	@brief	Find a sample that has a timestamp at least [deltaTime] from given start point
  *
  *	@param[in]		pEntry		- the TP object
  *	@param[in]		iStart		- start point
  *	@param[in]		deltaTime	- given time displacement
  *
  * @return			Index of a sample
  */
EV_INLINE
BYTE ev_find_point_by_time(PAlgoTPEntry pEntry, const BYTE iStart, const ULONG deltaTime)
{
	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, iStart < pEntry->count_tp);
	ULONGLONG t = pEntry->samples[iStart].rs_time + deltaTime;
	for (BYTE i = iStart + 1; i < pEntry->count_tp; i++)
	{
		if (pEntry->samples[i].rs_time > t)
			return i;
	}
	return (BYTE) (pEntry->count_tp - 1);
}


/**
  *	@brief	Find a sample that has a derivative by voltage less than [derMax]
  *
  *	@param[in]		pEntry	- the TP object
  *	@param[in]		iStart	- start point
  *	@param[in]		derMax	- maximum of derivative by voltage
  *
  * @return			If found, the index of a sample; if not - "count_tp"
  */
EV_INLINE
BYTE ev_find_point_by_der_v(PAlgoTPEntry pEntry, const BYTE iStart, const float derMax)
{
	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, iStart < pEntry->count_tp);
	for (BYTE i = iStart; i < pEntry->count_tp - 2; i++)
	{
		if (ABS(ev_der_v(pEntry, i)) < derMax)
			return i;
	}
	return pEntry->count_tp;
}

/**
*	@brief	Find a sample that has a derivative by current less than [derMax]
*
*	@param[in]		pEntry	- the TP object
*	@param[in]		iStart	- start point
*	@param[in]		derMax	- maximum of derivative by current
*
* @return			If found, the index of a sample; if not - "count_tp"
*/
EV_INLINE
BYTE ev_find_point_by_der_i(PAlgoTPEntry pEntry, const BYTE iStart, const float derMax)
{
	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, iStart < pEntry->count_tp);
	for (BYTE i = iStart; i < pEntry->count_tp - 2; i++)
	{
		if (ABS(ev_der_i(pEntry, i)) < derMax)
			return i;
	}
	return pEntry->count_tp;
}


/**
  * @brief	Calculates average and R2 value
  *
  * @param[in]      D		- the array of values
  * @param[in]      count	- the dimention of the array
  * @param[in,out]	pAv		- the pointer to struc EV_AVERAGE - output parameter
  */
EV_INLINE
void ev_average(double D[], const BYTE count, PEV_AVERAGE pAV)
{
	double offset, average, sum = 0, sum2 = 0, sum3 = 0;
	BYTE i;
	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, 0 < count);

	for (i = 0; i < count; i++)
		sum += D[i];
	average = sum / count;

	for (i = 0; i < count; i++)
	{
		offset = D[i] - average;
		sum2  += offset * offset;
		sum3   = D[i] * D[i];
	}

	pAV->avg_x	= average;
	pAV->sum_x2	= sum2;
	pAV->r2		= 1.0 - sum2 / sum3;
}


/**
  * @brief	Calculates average voltage/current value
  *
  * @param[in]      pEntry		- the TP object
  * @param[in]      iStart		- the starting point to do average
  * @param[in]      iEnd		- the ending point to do average
  * @param[in,out]	pAv			- the pointer to struc EV_AVERAGE - output parameter
  */
EV_INLINE
void ev_average_v(PAlgoTPEntry pEntry, const BYTE iStart, const BYTE iEnd, PEV_AVERAGE pAV)
{
	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, (0 <= iStart) && (iStart < iEnd) && (iEnd < pEntry->count_tp) && (iEnd < EV_TP_SAMPLE_COUNT));
	double D[EV_TP_SAMPLE_COUNT];
	for (BYTE i = iStart; i < iEnd; i++)
		D[i - iStart] = pEntry->samples[i].voltagea[g.iv];
	ev_average(D, iEnd - iStart, pAV);
}

EV_INLINE
void ev_average_i(PAlgoTPEntry pEntry, const BYTE iStart, const BYTE iEnd, PEV_AVERAGE pAV)
{
	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, (0 <= iStart) && (iStart < iEnd) && (iEnd < pEntry->count_tp) && (iEnd < EV_TP_SAMPLE_COUNT));
	double D[EV_TP_SAMPLE_COUNT];
	for (BYTE i = iStart; i < iEnd; i++)
		D[i - iStart] = pEntry->samples[i].current;
	ev_average(D, iEnd - iStart, pAV);
}


/**
  * @brief	Calculates linear regression and R2 value
  *
  * @param[in]      D		- the array of Y - values
  * @param[in]      T		- the array of X - values
  * @param[in]      count	- the dimension of the array
  * @param[in,out]	pLR		- the pointer to struct EV_LINEAR_REGR - output parameter
  *
  * @return TRUE, if linear regression is calculated; FALSE else and pLR contains zeros
  */
EV_INLINE
BOOL ev_linear_regression(double D[], double T[], const BYTE count, PEV_LINEAR_REGR pLR)
{
	double diff_e, sum_e2 = 0, diff_y, sum_y2 = 0;
	double E[6], R[2];
	BYTE i;
	ev_zeromem(pLR, sizeof(EV_LINEAR_REGR));
	ev_zeromem(&E[0], sizeof(E));

	//sum the samples value on both axes
	for (i = 0; i < count; i++)
	{
		E[0] += T[i] * T[i];
		E[1] += T[i];
		E[2] += T[i] * D[i];
		E[5] += D[i];
	}
	E[3] = E[1];
	E[4] = count;
	if (!ev_solve_2x2(E, R))
		return FALSE;

	// Calculate the slope(dV/dt) and output it
	pLR->slope  = R[0];
	pLR->value0 = R[1];
	pLR->avg_x  = E[1] / count;
	pLR->avg_y  = E[5] / count;

	// R2
	for (i = 0; i < count; i++)
	{
		diff_e  = D[i] - (T[i] * R[0] + R[1]);
		sum_e2 += diff_e * diff_e;
		diff_y  = D[i] - pLR->avg_y;
		sum_y2 += diff_y * diff_y;
	}
	if (sum_y2 < c.goZeroDivision)
		pLR->r2 = 0.99F;
	else
		pLR->r2 = 1.0 - sum_e2 / sum_y2;
	return TRUE;
}


/**
  * @brief	Calculates linear regression of voltage and some additions (see struct EV_LINEAR_REGR)
  *
  * @param[in]      pEntry		- the TP object
  * @param[in]      iStart		- the starting point of the linear regression calculation
  * @param[in]      iEnd		- the ending point - the linear regression DOES NOT INCLUDE this
  * @param[in,out]	pLR			- the pointer to struc EV_LINEAR_REGR - output parameter
  *
  * @return TRUE, if linear regression is calculated; FALSE else and pLR contains zeros
  */
EV_INLINE
BOOL ev_liregr_v(PAlgoTPEntry pEntry, const BYTE iStart, const BYTE iEnd, PEV_LINEAR_REGR pLR)
{
	double D[EV_TP_SAMPLE_COUNT], T[EV_TP_SAMPLE_COUNT];
	BYTE  i, is, count;

	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, (0 <= iStart) && (iStart + 1 < iEnd) && (iEnd <= pEntry->count_tp) && (iEnd <= EV_TP_SAMPLE_COUNT));
	count = iEnd - iStart;
	for (i = iStart; i < iEnd; i++)
	{
		is = i - iStart;
		D[is] = pEntry->samples[i].voltagea[g.iv];
		T[is] = (double) pEntry->samples[i].rs_time;
	}
	return ev_linear_regression(D, T, count, pLR);
}

/**
  * @brief	Calculates linear regression of current and some additions (see struct EV_LINEAR_REGR)
  *
  * @param[in]      pEntry		- the TP object
  * @param[in]      iStart		- the starting point of the linear regression calculation
  * @param[in]      iEnd		- the ending point - the linear regression DOES NOT INCLUDE this
  * @param[in,out]	pLR			- the pointer to struct EV_LINEAR_REGR - output parameter
  */
EV_INLINE
void ev_liregr_i(PAlgoTPEntry pEntry, const BYTE iStart, const BYTE iEnd, PEV_LINEAR_REGR pLR)
{
	double D[EV_TP_SAMPLE_COUNT], T[EV_TP_SAMPLE_COUNT];
	BYTE  i, is, count;

	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, (0 <= iStart) && (iStart + 1 < iEnd) && (iEnd <= pEntry->count_tp) && (iEnd <= EV_TP_SAMPLE_COUNT));
	count = iEnd - iStart;
	for (i = iStart; i < iEnd; i++)
	{
		is = i - iStart;
		D[is] = pEntry->samples[i].current;
		T[is] = (double) pEntry->samples[i].rs_time;
	}
	ev_linear_regression(D, T, count, pLR);
}

/**
	Take one Transition Period and calculate curve fitting.

	(x1, y1), (x2, y2), …, (xN, yN) – sample points
	We want approximate samples with function:
	F(x) = A * FA(x) + B * FB(x) + C * FC(x)

	A, B, C are the roots of the 3 equation with 3 unknowns:

	A * FA(xi)2       + B * FB(xi)*FA(xi) + C * FC(xi)*FA(xi) = yi*FA(xi)
	A * FA(xi)*FB(xi) + B * FB(xi)2       + C * FC(xi)*FB(xi) = yi*FB(xi)
	A * FA(xi)*FC(xi) + B * FB(xi)*FC(xi) + C * FC(xi)2       = yi*FC(xi)
*/

/**
  * @brief	Calculate Approximation. V(t),I(t) = A * t2 + B * t + C
  *
  * @param[in]      pEntry	- the TP object
  * @param[in]      iStart	- the starting point of the linear regression calculation
  * @param[in]      iEnd	- the ending point   of the linear regression calculation (excluded)
  * @param[in]      select	- selector: for voltage (TRUE) or for current (FALSE)
  * @param[in]      t0		- shift time (shift the axis Time)
  * @param[in,out]	pApp	- the pointer to struct EV_APPROX - output parameter
  *
  * @return	Either calculation succeeded
  */
EV_INLINE
BOOL ev_approx(PAlgoTPEntry pEntry, const BYTE iStart, const BYTE iEnd, const BOOL select_VorI, const BOOL select_3or2, double t0, PEV_APPROX pApp)
{
	double	E[12], x, y, fa, fb;
	double  ave_y, diff_y, sum_y2 = 0, diff_e, sum_e2 = 0;
	BYTE	i;

	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, (1 < iStart) && (iStart + 1 < iEnd) && (iEnd <= pEntry->count_tp));
	EV_ASSERT(EV_ERROR_CALC_WRONG_INPUT, (t0 <= pEntry->samples[iStart].rs_time) && (t0 >= pEntry->samples[iStart - 1].rs_time));
	ev_zeromem(&E[0], sizeof(E));
	ev_zeromem(pApp, sizeof(EV_APPROX));


	/*********************************************/
	for (i = iStart; i < iEnd; i++)
	{
		x = (double) pEntry->samples[i].rs_time - t0;
		y = select_VorI ? pEntry->samples[i].voltagea[g.iv] : pEntry->samples[i].current;
		//assert(x > c.goZeroDivision);
		if (x <= c.goZeroDivision)
		{
			EV_ERROR1(EV_ERROR_APPROXIMATION_FAILED);
			return FALSE;
		}

		fb = 1.0F / x;
		fa = fb / x;
		E[0] += fa * fa;
		E[1] += fa * fb;
		E[2] += fa;
		E[3] += y * fa;
		E[5] += fb * fb;
		E[6] += fb;
		E[7] += y * fb;
		E[11] += y;
	}
	E[ 4] = E[1];
	E[ 8] = E[2];
	E[ 9] = E[6];
	E[10] = iEnd - iStart;

	if (!select_3or2)
	{
		E[4] = E[6] = E[7] = 0;
		E[5] = 1.0;
	}

	if (!ev_solve_3x3(E, &pApp->coefA))
	{
		EV_ERROR1(EV_ERROR_APPROXIMATION_FAILED);
		return FALSE;
	}
	/*********************************************/

	// Calculate R2
	ave_y = E[11] / (iEnd - iStart);
	for (i = iStart; i < iEnd; i++)
	{
		x = (double) pEntry->samples[i].rs_time - t0;
		y = select_VorI ? pEntry->samples[i].voltagea[g.iv] : pEntry->samples[i].current;
		diff_y  = y - ave_y;
		sum_y2 += diff_y * diff_y;
		diff_e  = pApp->coefA / x / x + pApp->coefB / x + pApp->coefC - y;
		sum_e2 += diff_e * diff_e;
	}
	pApp->t0 = t0;
	pApp->r2 = 1.0 - sum_e2 / sum_y2;
	return TRUE;
}

#define DIVIDER_LEVEL 10
/**
  * @brief	Calculate Approximation. I(t) = A * t2 + B * t + C
  *
  * @param[in]      pEntry	- the TP object
  * @param[in]      iStart	- the starting point of the linear regression calculation
  * @param[in]      iEnd	- the ending point   of the linear regression calculation
  * @param[in,out]	pApp	- the pointer to structure EV_APPROX - output parameter
  *
  * @return	Either calculation succeeded
  */
EV_INLINE
BOOL ev_approximate(PAlgoTPEntry pEntry, const BYTE iStart, const BYTE iEnd, const BOOL select_VorI, const BOOL select_3or2, PEV_APPROX pApp)
{
	EV_APPROX	Approx[DIVIDER_LEVEL];
	BYTE		i, k, iMaxR2 = 0;
	double		dt0, t0, t1;
	if ((0 >= iStart) || (iStart >= iEnd) || (iEnd > pEntry->count_tp))
	{
		EV_ERROR1(EV_ERROR_CALC_WRONG_INPUT);
		return FALSE;
	}

	t0 = (double) pEntry->samples[iStart - 1].rs_time;
	t1 = (double) pEntry->samples[iStart    ].rs_time;
	for (k = 0; k < 2; k++)
	{
		// Divide an interval t0-t1 for a DIVIDER_LEVEL parts
		// and calculate the approximation DIVIDER_LEVEL times
		dt0 = (t1 - t0) / DIVIDER_LEVEL;
		for (i = 0; i < DIVIDER_LEVEL; i++) {
			ev_approx(pEntry, iStart, iEnd, select_VorI, select_3or2, t0 + dt0 * i, &Approx[i]);
		}

		// Take the index where R2 is maximum
		iMaxR2 = 0;
		for (i = 1; i < DIVIDER_LEVEL; i++) {
			if (Approx[i].r2 > Approx[iMaxR2].r2)
				iMaxR2 = i;
		}

		if (iMaxR2 == 0) {
			// Maximum is in the beginning (t0 is same)
			t1 = t0 + dt0 + dt0;
		}
		else if (iMaxR2 == DIVIDER_LEVEL - 1) {
			// Maximum is at the end (t1 is same)
			t0 = t1 - dt0 - dt0;
		}
		else {
			// Maximum is within the interval
			t0 = t0 + dt0 * (iMaxR2 - 1);
			t1 = t0 + dt0 + dt0;
		}
	}
	*pApp = Approx[iMaxR2];
	return TRUE;
}



#ifdef EV_CALC_DEBUG
EV_INLINE
BOOL ev_test_solve_3x3()
{
	double zero = 1E-12;
	double E[12] = { 3, 12, -4, 15,  2, 2, 3, 15,  1, -1, 2, 5 }; // Roots are (1.0, 2.0, 3.0)
	double R[3];
	BOOL match, matchA;

	ev_solve_3x3_A(E, R);
	matchA =
		(ABS(R[0] - 1.0) < zero) &&
		(ABS(R[1] - 2.0) < zero) &&
		(ABS(R[2] - 3.0) < zero)  ;

	ev_solve_3x3(E, R);
	match =
		(ABS(R[0] - 1.0) < zero) &&
		(ABS(R[1] - 2.0) < zero) &&
		(ABS(R[2] - 3.0) < zero)  ;

	return match && matchA;
}


EV_INLINE
BOOL ev_test_solve_2x2()
{
	double zero = c.goZeroDivision;
	double E[12] = { 1, 2, 5, 2, 3, 8 }; // Roots are (1.0, 2.0)
	double R[2];
	BOOL match;

	ev_solve_2x2(E, R);
	match =
		(ABS(R[0] - 1.0F) < zero) &&
		(ABS(R[1] - 2.0F) < zero)  ;
	return match;
}


EV_INLINE
BOOL ev_test_average()
{
	static const EV_RAW_DATA_SAMPLE testSamples[] =
	{
		{ 10, 3 * 10 + 5, { 100 } },
		{ 20, 3 * 20 + 5, { 100 } },
		{ 30, 3 * 30 + 5, { 100 } },
		{ 40, 3 * 40 + 5, { 100 } },
		{ 50, 3 * 50 + 5, { 100 } },
		{ 60, 3 * 60 + 5, { 100 } },
		{ 70, 3 * 70 + 5, { 100 } },
		{ 80, 3 * 80 + 5, { 100 } },
		{ 90, 3 * 90 + 5, { 100 } },
		{100, 3 * 100+ 5, {  90 } }
	};

	EV_LINEAR_REGR lr;
	EV_AVERAGE av;
	AlgoTPEntry entry;
	ev_zeromem(&entry, sizeof(entry));
	entry.count_tp = 10;
	ev_copymem(entry.samples, testSamples, 10 * sizeof(EV_RAW_DATA_SAMPLE));

	ev_average_v(&entry, 2, 8, &av);
	ev_average_i(&entry, 2, 8, &av);

	ev_liregr_v(&entry, 0, 10, &lr);
	ev_liregr_i(&entry, 0, 10, &lr);
	return TRUE;
}


EV_INLINE
BOOL ev_test_approx()
{
	double x;
	EV_APPROX approx;
	AlgoTPEntry entry;
	ev_zeromem(&entry, sizeof(entry));

	entry.count_tp = 20;
	for (BYTE i = 0; i < 20; i++)
	{
		x = i - 2.333;
		entry.samples[i].rs_time = i;
		if (i < 3)
			entry.samples[i].current = 20155, entry.samples[i].voltagea[g.iv] = 6857;
		else
			entry.samples[i].voltagea[g.iv] = (int) (3000.0 / x / x + 70.0 / x + 9.0 + 0.5),
			entry.samples[i].current = (int) (9000.0 / x / x + 50.0 / x + 1.0 + 0.5);
	}

	ev_approximate(&entry, 3, 20, TRUE , TRUE ,            &approx);
	ev_approx     (&entry, 3, 20, TRUE , FALSE, approx.t0, &approx);

	ev_approximate(&entry, 3, 20, FALSE, TRUE ,            &approx);
	ev_approx     (&entry, 3, 20, FALSE, FALSE, approx.t0, &approx);

	return TRUE;
}


EV_INLINE
BOOL ev_test_solve_quadratic()
{
	double R[3] = { 1, -5, 6 }; // 1, -5, 4 => 5 + 3 / 2 => 4
	double Y = 2;
	double r = ev_solve_quadratic(Y, R);
	return (ABS(r - 0.25) < c.goZeroDivision);
}

#endif // EV_CALC_DEBUG

#endif // EV_ALGEBRA_H
