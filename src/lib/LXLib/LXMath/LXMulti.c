/* -----------------------------------------------------------------------
 * $Id: LXMath.c 1117 2010-06-02 21:39:48Z viari $
 * -----------------------------------------------------------------------
 * @file: LXMulti.c
 * @desc: multinomial distribution
 * 
 * @history:
 * @+ <Gloup> : Jan 10 : added to LXMath
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * multinomial distribution utilities<br>
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

/* ---------------------------------------------------- */
/* internal utilities                                   */
/* ---------------------------------------------------- */

/* ---------------------------------------- */
/* multinomial generator                    */
/* ---------------------------------------- */

static int sMultiGenerate(int n, int k, int m[], int nleft, int level,
                          int (*callback)(int n, int k, const int *m, void *arg),
                          void *arg) {
    if (level >= k-1) {
      m[k-1] = nleft;
      return callback(n, k, m, arg);
    }

    int i, ok = 1;
    for (i = nleft ; (i >= 0) && ok ; i--) {
        m[level] = i;
        ok = sMultiGenerate(n, k, m, nleft - i, level+1, callback, arg);
    }
    
    return ok;
}

/* ---------------------------------------------------- */
/* API                                                  */
/* ---------------------------------------------------- */

/* ---------------------------------------- */

double LXLogMultinomialCoeff(int k, const int m[]) {
    int i, n;
    double s = 0;
    for (i = n = 0 ; i < k ; i++) {
        n += m[i];
        s += LXLogFactorial(m[i]);
    }
    return (LXLogFactorial(n) - s);
}

/* ---------------------------------------- */

double LXLogMultinomialPMF(int k, const int m[], const double p[]) {
    int i;
    double s = LXLogMultinomialCoeff(k, m);
    for (i = 0 ; i < k ; i++) {
        s += m[i] * log(p[i]);
    }
    return s;
}

/* ---------------------------------------- */
int LXMultinomialGenerator(int n, int k,
                           int (*callback)(int n, int k, const int *m, void *arg),
                           void *arg) {
    IFF_RETURN(n >= 0, LX_ARG_ERROR);
    IFF_RETURN(k >= 0, LX_ARG_ERROR);
    
    int *m = NEWN(int, n);
    IFF_RETURN(m, LX_MEM_ERROR);

    (void) sMultiGenerate(n, k, m, n, 0, callback, arg);
    
    FREE(m);

    return LX_NO_ERROR;
}

/* ---------------------------------------- */

double LXLogBinomialCoeff(int n, int m) {
    int mm[2] = {m, n-m};
    return LXLogMultinomialCoeff(2, mm);
}

/* ---------------------------------------- */

double LXLogBinomialPMF(int n, int m, double p) {
    int mm[2] = {m, n-m};
    double pp[2] = {p, 1. - p};
    return LXLogMultinomialPMF(2, mm, pp);
}

