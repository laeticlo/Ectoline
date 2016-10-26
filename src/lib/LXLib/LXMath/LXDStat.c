/* -----------------------------------------------------------------------
 * $Id: LXDStat.c 1782 2013-02-23 00:11:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXDstat.c
 * @desc: descriptive statistics utilities
 * 
 * @history:
 * @+ <Gloup> : Aug 06 : added to LXlib 
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * descriptive statistics utilities<br>
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "LX/LXSys.h"
#include "LX/LXMacros.h"
#include "LX/LXMath.h"

#include <limits.h>

/* ---------------------------------------------------- */
/* internal utilities                                   */
/* ---------------------------------------------------- */

/* ---------------------------------------- */
/* qsort comparison function                */
/* ---------------------------------------- */

static int sCompareFloat(const void *p1, const void *p2)
{
  const float *f1 = (const float *) p1;
  const float *f2 = (const float *) p2;

  if (*f1 == *f2)
    return 0;
    
  return (*f1 < *f2 ? -1 : 1);
}

static int sCompareDouble(const void *p1, const void *p2)
{
  const double *f1 = (const double *) p1;
  const double *f2 = (const double *) p2;

  if (*f1 == *f2)
    return 0;
    
  return (*f1 < *f2 ? -1 : 1);
}

/* ---------------------------------------------------- */
/* basic data manipulation                              */
/* ---------------------------------------------------- */

/* ---------------------------------------- */
/* duplicate data array                     */
/* ---------------------------------------- */

double *LXCopyData(double *data, size_t npt)
{
  double *newData = NEWN(double, npt);
  if (newData) {
    size_t i ;
    for (i = 0 ; i < npt ; i++)
      newData[i] = data[i];
  }
  return newData;
}

/* ---------------------------------------- */

float *LXCopyDataF(float *data, size_t npt)
{
  float *newData = NEWN(float, npt);
  if (newData) {
    size_t i ;
    for (i = 0 ; i < npt ; i++)
      newData[i] = data[i];
  }
  return newData;
}

/* ---------------------------------------- */
/* sort data                                */
/* ---------------------------------------- */

void LXSortData(double *data, size_t npt) {
  qsort(data, npt, sizeof(double), sCompareDouble);
}

/* ---------------------------------------- */

void LXSortDataF(float *data, size_t npt) {
  qsort(data, npt, sizeof(float), sCompareFloat);
}

/* ---------------------------------------------------- */
/* basic statistics                                     */
/* ---------------------------------------------------- */

/* ---------------------------------------- */
/* mean of data                             */
/* ---------------------------------------- */

double LXMean(double *data, size_t npt)
{
  size_t i;
  long double sum = 0.;
  
  for (i = 0 ; i < npt ; i++)
    sum += data[i];

  return (sum / (long double) npt);
}

/* ---------------------------------------- */

double LXMeanF(float *data, size_t npt)
{
  size_t i;
  long double sum = 0.;
  
  for (i = 0 ; i < npt ; i++)
    sum += data[i];

  return (sum / (long double) npt);
}

/* ---------------------------------------- */
/* variance of data                         */
/* ---------------------------------------- */

double LXVariance(double *data, size_t npt)
{
  size_t i;

  double mean = LXMean(data, npt);

  double variance = 0.;

  for (i = 0 ; i < npt ; i++) {
    double x = data[i] - mean;
    variance += ((x * x) - variance) / (double) (i + 1);
  }

  return variance * (double) npt / (double) (npt - 1);
}

double LXVarianceF(float *data, size_t npt)
{
  size_t i;

  double mean = LXMeanF(data, npt);

  double variance = 0.;

  for (i = 0 ; i < npt ; i++) {
    double x = data[i] - mean;
    variance += ((x * x) - variance) / (double) (i + 1);
  }

  return variance * (double) npt / (double) (npt - 1);
}

/* ---------------------------------------- */
/* standard deviation of data               */
/* ---------------------------------------- */

double LXStdDeviation(double *data, size_t npt)
{
  return sqrt(LXVariance(data, npt));
}

double LXStdDeviationF(float *data, size_t npt)
{
  return sqrt(LXVarianceF(data, npt));
}

/* ---------------------------------------- */
/* quantile of data                         */
/* ---------------------------------------- */

double LXQuantile(double *data, size_t npt, float quant, Bool alreadySorted)
{
  double index, delta, res;
  size_t lhs;
  double *sortedData;

  if (npt == 0)
    return 0.;
    
  if (! alreadySorted) {
    sortedData = LXCopyData(data, npt);
    LXSortData(sortedData, npt);
  }

  index = quant * (double) (npt - 1);
  lhs = (size_t) index;
  delta = index - lhs;

  if (lhs == npt - 1) {
    res = sortedData[lhs];
  }
  else {
    res = (1 - delta) * sortedData[lhs] + delta * sortedData[lhs + 1];
  }

  if (! alreadySorted) {
    FREE(sortedData);
  }
  
  return res;
}

double LXQuantileF(float *data, size_t npt, float quant, Bool alreadySorted)
{
  double index, delta, res;
  size_t lhs;
  float  *sortedData;

  if (npt == 0)
    return 0.;
    
  if (! alreadySorted) {
    sortedData = LXCopyDataF(data, npt);
    LXSortDataF(sortedData, npt);
  }

  index = quant * (double) (npt - 1);
  lhs = (size_t) index;
  delta = index - lhs;

  if (lhs == npt - 1) {
    res = sortedData[lhs];
  }
  else {
    res = (1 - delta) * sortedData[lhs] + delta * sortedData[lhs + 1];
  }

  if (! alreadySorted) {
    FREE(sortedData);
  }
  
  return res;
}

/* ---------------------------------------- */
/* init statistical structure               */
/* ---------------------------------------- */

void LXInitStat(LXStat *stat)
{
  stat->npt   = 0;
  stat->sum   = 0.;
  stat->sum2  = 0.;
  
  stat->mean  = 0.;
  stat->var   = 0.;
  stat->stdev = 0.;
  
  stat->min   = 0.;
  stat->max   = 0.;
}

/* ---------------------------------------- */
/* update statistical structure             */
/* ---------------------------------------- */

void LXUpdateStat(LXStat *stat, double value)
{
  stat->sum  += value;
  stat->sum2 += (value * value);

  stat->min = (stat->npt ? MIN(stat->min, value) : value);
  stat->max = (stat->npt ? MAX(stat->max, value) : value);
  
  stat->npt++;
}

/* ---------------------------------------- */
/* compute statistical structure            */
/* ---------------------------------------- */

int LXComputeStat(LXStat *stat)
{
  if (stat->npt <= 1)
    return LX_ARG_ERROR;
    
  stat->mean = stat->sum / (double) stat->npt;
  stat->var = (stat->sum2 - stat->mean * stat->sum) / (double) (stat->npt - 1);
  stat->stdev = sqrt(stat->var);
  
  return LX_NO_ERROR;
}


