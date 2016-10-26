/* -----------------------------------------------------------------------
 * $Id: LXMath.c 1117 2010-06-02 21:39:48Z viari $
 * -----------------------------------------------------------------------
 * @file: LXHyper.c
 * @desc: hypergeometric distribution 
 * 
 * @history:
 * @+ <Gloup> : Jan 10 : added to LXMath
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * multinomial distribution utilities<br>
 * including Fisher test<br>
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

#define EPSILON 1e-12

/* ---------------------------------------------------- */
/* internal utilities                                   */
/* ---------------------------------------------------- */

/* ----------------------------------------
 *    
 *   compute exact probability P(Obs) of observed table
 *       a b
 *       c d
 *                 r1! r2! c1! c2!
 *      P(Obs)  =  -----------------  (hypergeometric distribution)
 *                 a! b! c! d! n!
 *
 */

static double sPTable(int a, int b, int c, int d) {
    return exp(LXLogContingencyPMF(a, b, c, d));
}

/* ----------------------------------------
 *    """
 *    compute exact one tail pvalues P(<=a) and P(>=a) of table
 *      a b
 *      c d
 *    where a is <bold>the lowest frequency</bold> in table
 *    
 *    return the tuple (P(a), P(<=a), P(>=a))
 *    
 *    P(<=a) = Sum (Pi)
 *              i = 0, a
 *    
 *    P(>=a) = Sum (Pi)
 *              i = a, a + min(b,c)
 *    
 *    the trick to avoid recomputation of all ptable is the following :
 *    
 *    when a is decreased by 1, the new table becomes :
 *    a-1  b+1 
 *    c+1  d-1
 *    
 *    and the new ptable is therefore :
 *    P = (a+b)! (c+d)! (a+c)! (b+d) ! / (a-1)! (b+1)! (c+1)! (d-1)! (a+b+c+d)!
 *      = P0 * a * d / ((b+1) * (c+1))
 *    
 *    when a is increased by 1, the new table becomes :
 *    a+1  b-1 
 *    c-1  d+1
 *    
 *    and the new ptable is therefore :
 *    P = (a+b)! (c+d)! (a+c)! (b+d) ! / (a+1)! (b-1)! (c-1)! (d+1)! (a+b+c+d)!
 *      = P0 * b * c / ((a+1) * (d+1))
 *    
 *    note: usually this trick has some risk of underflow (when the
 *    starting P(a) is very small (basically 0.) and if
 *    we are climbing the distribution upward. For this reason, we 
 *    always go downwards the distribution, this ensures the rounding
 *    error due to summation will decrease.
 *    note : we additionally stop the computation when the values 
 *    becomes too small to impact the precision. This is evaluated
 *    by an upperbound of the remaining sum : 
 *        remaining_sum <= current_p * nb_remaining_points
 *    and we stop if :
 *        remaining_sum < EPSILON * p0 < EPSILON * pvalue
 *
 *    note: a EPSILON value less than 1E-12 is meaningless since :
 *        -1- the computation is done using doubles.
 *        -2- the Lanczos formula has a precision of about 1e-12.
 *    However a greater value may result in a (slight) increase in speed
 *
 */

static void sPdown(int a, int b, int c, int d,
                     double *pleft, double *pright, double *pobs) {
    //
    // compute the first proba (highest value)
    //
    double s, p, p0;
    
    s = p = p0 = sPTable(a, b, c, d);

    *pobs = p0;
    
    double prec = p0 * EPSILON; // required precision
    
    //
    // then goes downwards the distribution
    //
    // the mode is a0 = (a+b)(a+c)/n
    //
    // therefore
    // a >= a0 : goto right
    // a < a0  : goto left
    //
    int a0 = roundl((double) (a+b) * (double) (a+c) / (double) (a+b+c+d));

    if (a >= a0) {            // go to right
        int m = a + MIN(b, c);
        while (a <= m) {
            a++;
            d++;
            p *= (double) (b * c) / (double) (a * d);
            s += p;
            if (((m-a) * p) < prec)
                break;       //  useless to continue
            b--;
            c--;
        }
        *pright = s;
        *pleft = 1. - s + p0;
    }        
    else {                  // go to left
        while (a >= 0) {
            b++;
            c++;
            p *= (double) (a * d) / (double) (b * c);
            s += p;
            if ((a * p) < prec)
                break;      // useless to continue
            a--;
            d--;
        }
        *pleft = s;
        *pright = 1. - s + p0;
    }
}

/* ---------------------------------------------------- */
/* API                                                  */
/* ---------------------------------------------------- */

/* ---------------------------------------- */

double LXLogMultiHyperPMF(int k, const int M[], const int m[]) {
    int i, n = 0, N = 0;
    double s = 0;

    for (i = 0 ; i < k ; i++) {
        s += LXLogBinomialCoeff(M[i], m[i]);
        n += m[i];
        N += M[i];
    }

    return (s - LXLogBinomialCoeff(N, n));
}

/* ---------------------------------------- */

double LXLogHyperPMF(int N, int n, int M, int m) {
    int mm[2] = {m, n-m};
    int MM[2] = {M, N-M};
    return LXLogMultiHyperPMF(2, MM, mm);
}

/* ---------------------------------------- */

double LXLogContingencyPMF(int a, int b, int c, int d) {
    return LXLogHyperPMF(a+b+c+d, a+c, a+b, a);
}

/* ---------------------------------------- */

int LXFisherOneTail(int a, int b, int c, int d,
                       double *pleft, double *pright, double *pobs) {

    double pl, pr, po;
    int minf = MIN(MIN(a, b), MIN(c, d));
    
    if (minf == a) 
        sPdown(a, b, c, d, &pl, &pr, &po);
    else if (minf == b) 
        sPdown(b, a, d, c, &pl, &pr, &po); // note both inversions
    else if (minf == c) 
        sPdown(c, d, a, b, &pl, &pr, &po); // note both inversions
    else
        sPdown(d, c, b, a, &pl, &pr, &po);

    if (pobs)   *pobs   = po;    
    if (pleft)  *pleft  = pl;    
    if (pright) *pright = pr;    

    return LX_NO_ERROR;
}



