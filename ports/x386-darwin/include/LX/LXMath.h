/* -----------------------------------------------------------------------
 * $Id: LXMath.h 1126 2010-06-13 20:09:50Z viari $
 * -----------------------------------------------------------------------
 * @file: LXMath.h
 * @desc: math & stat utilities
 * 
 * @history:
 * @+ <Gloup> : Jan 96 : first version for PWG 
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * math & stat utilities<br>
 *
 * @docend:
 */

#ifndef _H_LXMath

#define _H_LXMath

#ifndef _H_LXTypes
#include "LXTypes.h"
#endif

#ifndef _H_LXMacros
#include "LXMacros.h"
#endif

/* ---------------------------------------------------- */
/* Constants  											*/
/* ---------------------------------------------------- */

/* ---------------------------------------------------- */
/* Structures  											*/
/* ---------------------------------------------------- */

/*
 * @doc: statistics storage
 * - npt     : number of data values
 * - mean    : mean of values
 * - var     : variance of values
 * - stdev   : standard deviation of values = sqrt(var)
 * - min     : minimum of values
 * - max     : maximum of values
 * - sum     : - internal - sum of values
 * - sum2    : - internal - sum of squared values
 */

typedef struct {
  size_t npt;
  double mean;
  double var;
  double stdev;
  double min;
  double max;
  long double sum;
  long double sum2;
} LXStat;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/* -------------------------------- */
/* LXDStat.c                        */
/* -------------------------------- */

/*
 * @doc: 
 * copy data array
 * @param double* or float* data values
 * @param size_t npt number of values
 * @return a copy array of data values
 */

double *LXCopyData(double *data, size_t npt);
float  *LXCopyDataF(float *data, size_t npt);

/*
 * @doc: 
 * sort data values (ascending order)
 * @param double* or float* data values
 * @param size_t npt number of values
 */

void LXSortData(double *data, size_t npt);
void LXSortDataF(float *data, size_t npt);

/*
 * @doc: 
 * mean of data values
 * @param double* or float* data values
 * @param size_t npt number of values
 * @return double mean
 * @see also LXInitStat/LXUpdateStat/LXComputeStat 
 */

double LXMean(double *data, size_t npt);
double LXMeanF(float *data, size_t npt);

/*
 * @doc: 
 * variance of data values
 * @param double* or float* data values
 * @param size_t npt number of values
 * @return double mean
 * @see also LXInitStat/LXUpdateStat/LXComputeStat 
 */

double LXVariance(double *data, size_t npt);
double LXVarianceF(float *data, size_t npt);

/*
 * @doc: 
 * standard deviation of data values
 * @param double* or float* data values
 * @param size_t npt number of values
 * @return double mean
 * @see also LXInitStat/LXUpdateStat/LXComputeStat 
 */

double LXStdDeviation(double *data, size_t npt);
double LXStdDeviationF(float *data, size_t npt);

/*
 * @doc: 
 * quantile 'quant' of data values
 * @param double* or float* data values
 * @param size_t npt number of values
 * @param float quant (quantile (in ]0., 1.[)
 * @param Bool alreadySorted true if data is already sorted
 *        (please see LXSortData()). If set to false data will
 *        be internally sorted (your original data is unchanged)
 * @return quantile
 */

double LXQuantile(double *data, size_t npt, float quant, Bool alreadySorted);
double LXQuantileF(float *data, size_t npt, float quant, Bool alreadySorted);

/*
 * @doc: 
 * init statistical structure
 * @param LXStat *stat stat structure to initialize
 */

void LXInitStat(LXStat *stat);

/*
 * @doc: 
 * update statistical structure with value
 * @param LXStat *stat stat structure to update
 */

void LXUpdateStat(LXStat *stat, double value);

/*
 * @doc: 
 * compute mean and var of statistical structure
 * @param LXStat *stat stat structure to finalize
 * @return LX_NO_ERROR or LX_ARG_ERROR
 */

int LXComputeStat(LXStat *stat);

/* -------------------------------- */
/* LXFact.c                         */
/* -------------------------------- */

/*
 * @doc: 
 * compute ln(gamma(x)) using Lanczos formula.
 * precision is about 1e-12
 * @param x double
 * @return ln(gamma(x))
 */

double LXLogGamma(double x);

/*
 * @doc: 
 * compute ln(n!) using LXLogGamma
 * @param n positive or null integer
 * @return ln(n !)
 */

double LXLogFactorial(int n);

/*
 * @doc: 
 * permutation generator. generate all (n!) permutations of n elements.
 * for each permutation s[] call user's callback(n, s, arg). this callback
 * should return 1 to continue or 0 to stop the generation process.
 * s[] is an array of int containing the current permutation, i.e.
 * s[i=0,n-1] = j(=[0,n-1]). the first s[] is identity (s[i]=i) and
 * next s[] are produced by cycling on the highest indices. 
 * e.g. for n = 3, s[] are produced in the following order :
 * s = [0 1 2]; s = [0 2 1]; s = [1 0 2]; s = [1 2 0]; s = [2 0 1]; s = [2 1 0]
 *
 * @param n int
 * @param callback user's callback
 * @param arg void* optional user's callback argument
 * @return LX_NO_ERROR if ok or LX_ARG_ERROR, LX_MEM_ERROR on error
 */

int LXPermutationGenerator(int n, 
                           int (*action)(int n, const int *s, void *arg),
                           void *arg);

/* -------------------------------- */
/* LXMulti.c                        */
/* -------------------------------- */

/*
 * @doc: 
 * compute ln(MultinomialCoeff(n, m1, .., mk)) 
 * = ln(number of way to choose m1,..,mk elements amongst n = m1+...+mk).
 * = ln({n, m1,..., mk}) with n = Sum(i=0,k-1; mi)
 * = ln(n!/(m1! m2! ... mk!))
 * @param k int positive : number of classes
 * @param m[] array of k integer
 * @return ln(n!/(m1! m2! ... mk!))
 */

double LXLogMultinomialCoeff(int k, const int m[]);

/*
 * @doc: 
 * compute ln(Multinomial_Probability_Mass_Function) 
 * = ln(probability of drawing exactly mi elements, each of them
 *   with probability pi, in n draws with replacement.
 * = ln({n, m1, m2,...,mk!})*Prod(i=0,k-1; pi^mi)) 
 *   with n = Sum(i=0,k-1; mi)
 * = ln(n!/(m1! m2! ... mk!)*Prod(i=0,k-1; pi^mi)) 
 * @param k int positive : number of classes
 * @param m[] array of k positive integer : number of elements in each class
 * @param p[] array of k probas : probability of each class
 *            (with Sum(i=0,k-1;p[i]) = 1)
 * @return ln(Multinomial_Probability_Mass_Function)
 */

double LXLogMultinomialPMF(int k, const int m[], const double p[]);

/*
 * @doc: 
 * multinomial generator. generate all ways of choosing n elements
 * in k classes (multinomial combination).
 * for each combination m[] call user's callback(n, k, m, arg). this callback
 * should return 1 to continue or 0 to stop the generation process.
 * m[] is an array of k integer containing the number of elements in
 * each class (i.e. with Sum(i=0,k-1;m[i]) = n.
 * note: combinations m[] are generated in such a way that the lowest
 * indices get the highest possible frequencies.
 * e.g. for n = 5, k=3, m[] are produced in the following order :
 * m = [5 0 0];
 * m = [4 1 0]; m = [4 0 1];
 * m = [3 2 0]; m = [3 1 1]; m = [3 0 2];
 * m = [2 3 0]; m = [2 2 1]; m = [2 1 2]; m = [2 0 3];
 * m = [1 4 0]; m = [1 3 1]; m = [1 2 2]; m = [1 1 3]; m = [1 0 4];
 * m = [0 5 0]; m = [0 4 1]; m = [0 3 2]; m = [0 2 3]; m = [0 0 5]
 *
 * @param n int positive : total number of elements
 * @param k int positive : number of classes
 * @param callback user's callback
 * @param arg void* optional user's callback argument
 * @return LX_NO_ERROR if ok or LX_ARG_ERROR, LX_MEM_ERROR on error
 */

int LXMultinomialGenerator(int n, int k,
                           int (*callback)(int n, int k, const int *m, void *arg),
                           void *arg);

/*
 * @doc: 
 * compute ln(BinomialCoeff(n, k))
 * = ln(number of way of choosing m elements amongst n)
 * = ln({n, m})
 * = ln(n!/(m! (n-m)!))
 * @param n int positive : total number of elements
 * @param m int positive : number of elements to take
 * @return ln({n, m})
 */

double LXLogBinomialCoeff(int n, int m);

/*
 * @doc: 
 * compute ln(Binomial_Probability_Mass_Function)
 * = ln(probability of drawing exactly m elements, each of them
 *   with probability p, from n draws with replacement.
 * = ln({n, m}*p^m*(1-p)^(n-m))
 * = ln(n!/(m! (n-m)!)*p^m*(1-p)^(n-m))
 * @param n int positive : total number of draws
 * @param m int positive : number of elements drawn
 * @param p proba of event of frequency m
 * @return ln(Binomial_Probability_Mass_Function) 
 */

double LXLogBinomialPMF(int n, int m, double p);

/* -------------------------------- */
/* LXHyper.c                        */
/* -------------------------------- */

/*
 * @doc: 
 * compute ln(Multinomial_Hypergeometric_Probability_Mass_Function)
 * = ln(probability of drawing exactly mi elements, each of them
 *   with initial cardinality Mi, in n draws without replacement.
 * = ln(Prod(i=0,k-1; {Mi, mi}) / {N, n}) ; 
 *   n = Sum(i=0,k-1; mi) and N = Sum(i=0,k-1; Mi)
 *   and {n, k} = n!/k!(n-k)!
 * @param k int positive : number of classes
 * @param M[] array of k positive integer : initial number of elements in each class
 * @param m[] array of k positive integer : number of elements drawn in each class
 *            (with mi <= Mi)
 * @return ln(Multinomial_Hypergeometric_Probability_Mass_Function)
 */

double LXLogMultiHyperPMF(int k, const int M[], const int m[]);

/*
 * @doc: 
 * compute ln(Hypergeometric_Probability_Mass_Function)
 * = ln(probability of drawing exactly m elements, with initial cardinality M,
 *      in n draws without replacement from an urn of initial cardinality N.
 * = ln({M, m} {N-M, n-m) / {N, n}) ;  with {n, k} = n!/k!(n-k)!
 *
 * Note: this also equals to the probability of the contingency table :
 *         Drawn    notDrawn   margin
 * White   m        M-m        M
 * Black   n-m      N-M-n+m    N-M
 * margin  n        N-n        N
 * see LXLogContingency
 * @param N int positive : total initial number of elements
 * @param n int positive : total number of elements drawn
 * @param M int positive : initial cardinality of elements of interest
 * @param m int positive : number of elements of interest drawn
 * @return ln(Hypergeometric_Probability_Mass_Function)
 */

double LXLogHyperPMF(int N, int n, int M, int m); 

/*
 * @doc: 
 * compute ln(Probability of 2x2 contingency table {a, b, c, d})
 *                        (margin)  
 *             a     b     a+b
 *             c     d     c+d
 * (margin)   a+c   b+d    a+b+c+d
 *
 * = ln((a+b)! (c+d)! (a+c)! (b+d)! / (a+b+c+d)! a! b! c! d!)
 * = LXLogHyperPMF(a+b+c+d, a+c, a+b, a)
 *
 * Note: see also LXLogHyperPMF, LXFisherOneTail
 * @param a, b, c, d int positive : 2x2 contingency table
 * @return ln(Probability of contingency table {a, b, c, d})
 */

double LXLogContingencyPMF(int a, int b, int c, int d);

/*
 * @doc: 
 *   compute exact fisher one tail pvalues of contingency table
 *    a b
 *    c d
 *  
 *    pleft = P(Obs<=Exp)  = Sum (Pi)
 *                           i = 0, a
 *    pright = P(Obs>=Exp) = Sum (Pi)
 *                           i = a, a+min(b,c)
 *    pobs = P(Obs=Exp) = Pa
 *    
 *  all P's are double values with a default precision of about 10 digits
 *  and a range [1E-320, 1]  (this is the range of double)
 *  values less than 1E-320 will appear as 0.
 * @param a, b, c, d int positive : 2x2 contingency table
 * @param pleft, pright, pobs double* will hold results
 * @return LX_NO_ERROR if computation was ok
 */

int LXFisherOneTail(int a, int b, int c, int d,
                    double *pleft, double *pright, double *pobs);

#endif
