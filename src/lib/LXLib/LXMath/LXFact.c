/* -----------------------------------------------------------------------
 * $Id: LXMath.c 1117 2010-06-02 21:39:48Z viari $
 * -----------------------------------------------------------------------
 * @file: LXFact.c
 * @desc: factorial related utilities
 * 
 * @history:
 * @+ <Gloup> : Aug 06 : added to LXlib 
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * actorial related utilities<br>
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "LX/LXSys.h"
#include "LX/LXMacros.h"
#include "LX/LXMath.h"

/* ---------------------------------------------------- */
/* internal utilities                                   */
/* ---------------------------------------------------- */

/* ---------------------------------------- */
/* permutation generator                    */
/* ---------------------------------------- */
static int sPermutate(int n, int *s, char *u, int k,
                      int (*callback)(int n, const int *s, void *arg),
                      void *arg) {
              
    if (k >= n) {
      return callback(n, s, arg);
    }

    int i, ok = 1;
    for (i = 0 ; (i < n) && ok ; i++) {
        if (u[i]) continue;
        u[i] = 1;
        s[k] = i;
        ok = sPermutate(n, s, u, k+1, callback, arg);
        u[i] = 0;
    }
    
    return ok;
}

/* ---------------------------------------------------- */
/* API                                                  */
/* ---------------------------------------------------- */

/* ---------------------------------------- */
/* gamma function                           */
/* ---------------------------------------- */

double LXLogGamma(double x) {
    double s;
    s  = 0.1659470187408462e-06 / (x+7);
    s += 0.9934937113930748e-05 / (x+6);
    s -= 0.1385710331296526     / (x+5);
    s += 12.50734324009056      / (x+4);
    s -= 176.6150291498386      / (x+3);
    s += 771.3234287757674      / (x+2);
    s -= 1259.139216722289      / (x+1);
    s += 676.5203681218835      / (x);
    s += 0.9999999999995183;
    return log(s) - 5.58106146679532777 - x + (x-0.5) * log(x+6.5);
}

/* ---------------------------------------- */
/* factorial                                */
/* ---------------------------------------- */

double LXLogFactorial(int n) {
    return (n <= 1) ? 0. : LXLogGamma(n+1);
}

/* ---------------------------------------- */
/* permutation generator                    */
/* ---------------------------------------- */

int LXPermutationGenerator(int n, 
                           int (*callback)(int n, const int *s, void *arg),
                           void *arg) {

    IFF_RETURN(n > 0, LX_ARG_ERROR);
    
    int *s = NEWN(int, n);
    IFF_RETURN(s, LX_MEM_ERROR);

    char *u = NEWN(char, n);
    if (! u) {    
        FREE(s);
        return LX_MEM_ERROR;
    }

    (void) memset(u, 0, n);
    
    (void) sPermutate(n, s, u, 0, callback, arg);
    
    FREE(s);
    FREE(u);
    
    return LX_NO_ERROR;
}

