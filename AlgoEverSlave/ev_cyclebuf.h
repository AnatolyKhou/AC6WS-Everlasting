/*-----------------------------------------------------------------------------
 * Name:		ev_cyclebuf.h
 * Purpose:		The algorithm itself.
 * Revision:	02.00
 * Author:		Anatoly Kh.
 * Copyright:	(C) 2020 Algolion LTD. All rights reserved.
 *----------------------------------------------------------------------------*/
#ifndef EV_CYCLEBUF_H
#define EV_CYCLEBUF_H

#define EV_CYCLE_BUF_MASK	0x7F // Mask to ANDed an index for the "EV_GLOBALS.cb_samp" array, see EV_CYCLE_BUF_SIZE

/******************************************************************************
 * @brief		Get last sample (point) from the Cycle Buffer
 * @return		Address of a sample
 */
EV_INLINE PEV_RAW_DATA_SAMPLE
ev_cb_get_last()
{
	return g.cb_samp + ((g.uCountSample - 1) & EV_CYCLE_BUF_MASK);
}

/******************************************************************************
 * @brief		Get sample (point) from the Cycle Buffer,
 *              relatively point "J" - first or second Jump point of the TP
 * @return		Address of a sample
 */
EV_INLINE PEV_RAW_DATA_SAMPLE
ev_cb_get_samp(const LONG offset)
{
	LONG index = (g.uCountSample - g.offsetStart + offset) & EV_CYCLE_BUF_MASK;
	return g.cb_samp + index;
}


 /******************************************************************************
 * @brief			Calculate derivative of Electric Current (per cell)
 * @param[in]		pLast	- Data Raw sample, last one
 * @param[in]		pCurr	- Data Raw sample, current one
 * @return			The derivative value, in A/s
 */
EV_INLINE float
ev_cb_derivI(const PEV_RAW_DATA_SAMPLE pLast, const PEV_RAW_DATA_SAMPLE pCurr)
{
	float deriv = 100.0F * (pCurr->current - pLast->current) / (pCurr->rs_time - pLast->rs_time);
	// Units: 100 * dA * 1E+4 [Amperes] / dt * 1E+6 [seconds] = Amperes/seconds
	return deriv;
}


 /******************************************************************************
 * @brief			Calculate derivative of Voltage (per cell)
 * @param[in]		pLast	- Data Raw sample, last one
 * @param[in]		pCurr	- Data Raw sample, current one
 * @return			The derivative value, in V/s
 */
EV_INLINE float
ev_cb_derivV(const PEV_RAW_DATA_SAMPLE pLast, const PEV_RAW_DATA_SAMPLE pCurr)
{
	float derivAMax = 0, derivMax = 0;
	float dt = 100.0F / (pCurr->rs_time - pLast->rs_time);
	for (BYTE i = 0; i < EV_PACK_SUPERS_INMODULE; i++)
	{
		float deriv = ((LONG) pCurr->voltagea[i] - (LONG) pLast->voltagea[i]) * dt;
		// Units: 100 * dV * 1E+4 [Volt] / dt * 1E+6 [seconds] = Volt/seconds

		if (derivAMax < ABS(deriv))
		{
			derivAMax = ABS(deriv);
			derivMax  = deriv;
		}
	}
	return derivMax;
}


/******************************************************************************
 * @brief		Average a group of sample
 * @param[in]	    offsetFrom  - offset to the first sample (how many samples), inclusively
 * @param[in]	    offsetUpto  - offset to the last sample (how many samples), inclusively
 * @param[in]	    offsetCount - number of points to be average
 * @param[in]	    deltaOffset - step (should be +1 or -1)
 * @param[in,out]	pSample     - Address to place the results of average
 * @return		TRUE if success, FALSE else - bad input or the buffer is not full yet
 * @remark      Find a group of [c.offPointCount] samples of type "QCC" and
 *              average them
 */
EV_INLINE
BYTE ev_cb_get_average5(
    const LONG offsetFrom, const LONG offsetUpto, const LONG offsetCount, const LONG deltaOffset,
    BYTE flagStrong, const PEV_RAW_DATA_SAMPLE pSample)
{
    PEV_RAW_DATA_SAMPLE pLast;
    LONG fromIndex, uptoIndex, count = 0, i = 0;
//    BYTE type, typeToStop;
//    double DV[256], DI[256];
//    EV_AVERAGE AV;

    fromIndex = (offsetFrom + g.uCountSample - g.offsetStart) & EV_CYCLE_BUF_MASK;
    uptoIndex = (offsetUpto + g.uCountSample - g.offsetStart + deltaOffset) & EV_CYCLE_BUF_MASK;
    memset(pSample, 0, sizeof(EV_RAW_DATA_SAMPLE));
//    typeToStop = flagStrong ? EV_STYPE_JUMP | EV_STYPE_TIME_BREAK | EV_STYPE_SLOPE : EV_STYPE_JUMP | EV_STYPE_TIME_BREAK;

    while (fromIndex != uptoIndex)
    {
        pLast = g.cb_samp + fromIndex;
        //type = pLast->ctypeI | pLast->ctypeV;
        //assert(type != 0);
        //if ((type & typeToStop) != 0)
        //{
        //    EV_ERROR1(EV_ERROR_POINT_AD);
        //    break;
        //}
        count++;
        fromIndex = (fromIndex + deltaOffset) & EV_CYCLE_BUF_MASK;
    }
    if (fromIndex == uptoIndex)
        fromIndex = (fromIndex - deltaOffset) & EV_CYCLE_BUF_MASK;

    //assert(count > 0);
    //assert(count >= offsetCount);
    count = MIN(count, offsetCount);
    if (count == 0)
    {
        EV_ERROR1(EV_ERROR_POINT_AD_COUNT);
        return FALSE;
    }

    while(i < count)
    {
        fromIndex = (fromIndex - deltaOffset) & EV_CYCLE_BUF_MASK;
        pLast = g.cb_samp + fromIndex;
        pSample->rs_time += pLast->rs_time;
        for (BYTE k = 0; k < EV_PACK_SUPERS_INMODULE; k++)
        	pSample->voltagea[k] += pLast->voltagea[k];
        pSample->current += pLast->current;
//        DV[i] = pLast->voltage;
//        DI[i] = pLast->current;
        i++;
    }
    pSample->rs_time /= count;
    for (BYTE k = 0; k < EV_PACK_SUPERS_INMODULE; k++)
    	pSample->voltagea[k] /= count;
    pSample->current /= count;

//    ev_average(DV, (BYTE) i, &AV);
//    pSample->r2V = (Byte) (AV.r2 * 100.0);
//    ev_average(DI, (BYTE) i, &AV);
//    pSample->r2I = (Byte) (AV.r2 * 100.0);

    return TRUE;
}


/******************************************************************************
 * @brief		Average a group of sample to get point "A"
 * @param   	PAlgoTPEntry - TP object
 * @return		TRUE if success, FALSE else - bad input or the buffer is not full yet;
 *              If TRUE the results are in the TP object
 * @remark      How many samples to average is taken from [c.offPointCount];
 *              the offset is taken from [c.offPointALeft];
 */
EV_INLINE
BOOL ev_cb_get_pointA(PAlgoTPEntry pEntry)
{
	EV_RAW_DATA_SAMPLE sample;
    if (!ev_cb_get_average5(-1, - c.offPointALeft - 1, c.offPointCount, -1, c.flagStrongALeft, &sample))
        return FALSE;
    pEntry->qccV = sample.voltagea[g.iv];
    pEntry->qccI = sample.current;
//    if (sample.r2I >= c.tpDetQCCR2 * 100.0)
//        return FALSE;
//    if (sample.r2V >= c.tpDetQCCR2 * 100.0)
//        return FALSE;
    return TRUE;
}


/******************************************************************************
 * @brief		Average a group of sample to get point "D"
 * @param   	PAlgoTPEntry - TP object
 * @return		TRUE if success, FALSE else - bad input or the buffer is not full yet;
 *              If TRUE the results are in the TP object
 * @remark      How many samples to average is taken from [c.offPointCount];
 *              the offset is taken from [c.offPointDRight];
 */
EV_INLINE
BOOL ev_cb_get_pointD(PAlgoTPEntry pEntry)
{
	EV_RAW_DATA_SAMPLE sample;
    if (!ev_cb_get_average5(pEntry->count_jump + pEntry->count_slope,
                            pEntry->count_jump + pEntry->count_slope + c.offPointDRight,
                            c.offPointCount, +1, c.flagStrongDRight, &sample))
        return FALSE;
    pEntry->tailV = sample.voltagea[g.iv];
    pEntry->tailI = sample.current;
//   if (sample.r2I >= c.tpDetTailR2 * 100.0)
//        return FALSE;
//    if (sample.r2V >= c.tpDetTailR2 * 100.0)
//        return FALSE;
    return TRUE;
}


/******************************************************************************
 * @brief		Calculate Next index value for given index value (within the cycla buffer)
 * @param[in]	indexValue - index value
 * @return		Next index value
 */
EV_INLINE
void ev_cb_copy_samples(PAlgoTPEntry pEntry)
{
    ULONGLONG timeShift;
    PEV_RAW_DATA_SAMPLE pSample;
    LONG i, shift, count = pEntry->count_tp;

    assert((ev_cb_get_samp(0)->ctypeV | ev_cb_get_samp(0)->ctypeI) & EV_STYPE_JUMP);

    shift = MIN(pEntry->count_qcc, pEntry->count_qccI);
    for (i = 0; i < count; i++)
    {
        pSample = ev_cb_get_samp(i - shift);
        pEntry->samples[i] = *pSample;
    }
    timeShift = pEntry->samples[0].rs_time;
    for (i = 0; i < count; i++)
        pEntry->samples[i].rs_time -= timeShift;

    pEntry->time_tp = timeShift;
}


#endif // EV_CYCLEBUF_H
