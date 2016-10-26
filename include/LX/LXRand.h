/* -----------------------------------------------------------------------
 * $Id: LXRand.h 1793 2013-02-24 02:19:06Z viari $
 * -----------------------------------------------------------------------
 * @file: LXRand.h
 * @desc: random numbers utilities
 * 
 * @history:
 * @+ <Gloup> : Jan 96 : first version for PWG 
 * @+ <Gloup> : Jan 10 : random generators added 
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * random numbers utilities<br>
 *
 * @docend:
 */

#ifndef _H_LXRand

#define _H_LXRand

#ifndef _H_LXTypes
#include "LXTypes.h"
#endif

#ifndef _H_LXMacros
#include "LXMacros.h"
#endif

/* ---------------------------------------------------- */
/* Constants  											*/
/* ---------------------------------------------------- */

/*
 * @doc: 
 * generator selector : 
 *  LX_RAND_UNIX  
 *       Unix gclib builtin random number generator (random)
 *       period ~ 3.10^10
 *  LX_RAND_PARK_MILLER_BD 
 *       minimal Park and Miller generator with Bays-Durham shuffle
 *       period = 2^31 - 2 ~ 2.10^9 - good spectral prop.
 *  LX_RAND_LECUYER_BD
 *       long period generator of L'Ecuyer with Bays-Durham shuffle
 *       period = ~ 2.10^18 - good spectral prop.
 *  LX_RAND_KNUTH
 *       substractive random number generator of Knuth
 *       period ~ ?
 * LX_RAND_LCG_NUMREC
 *       linear congruential generator from Numrical Recipes
 *       period < 2^32 - bad spectral properties
 * LX_RAND_LCG_CARBON
 *       linear congruential generator used in Apple Carbon lib
 *       period < 2^32 - bad spectral properties
 * LX_RAND_LCG_JAVA
 *       linear congruential generator used in Apple Carbon lib
 *       period < 2^48 - bad spectral properties
 */
 
typedef enum {
    LX_RAND_UNIX = 0,
    LX_RAND_PARK_MILLER_BD,
    LX_RAND_LECUYER_BD,
    LX_RAND_KNUTH,
    LX_RAND_LCG_NUMREC,
    LX_RAND_LCG_CARBON,
    LX_RAND_LCG_JAVA
} LXRandomSelector;


/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * select a random generator 
 * @param selector LXRandomSelector
 *  LX_RAND_UNIX,
 *  LX_RAND_PARK_MILLER_BD,
 *  LX_RAND_LECUYER_BD,
 *  LX_RAND_KNUTH,
 *  LX_RAND_LCG_NUMREC,
 *  LX_RAND_LCG_CARBON,
 *  LX_RAND_LCG_JAVA
 */
void LXSelectRandom(LXRandomSelector selector);


/*
 * @doc: 
 * initialize the random number generator with seed
 * @param seed unsigned
 * if seed == 0 : use current time to make the seed
 */

void LXInitRandom(unsigned seed);

/*
 * @doc: 
 * returns a uniform random deviate between 0. and 1.
 * (exclusive of the endpoint values, that is in range
 * [DBL_MIN, 1-DBL_MIN]
 */

double LXUniformFRandom(void);

/*
 * @doc: 
 * return uniform random integer in [from, to[
 */

long LXUniformIRandom(long from, long to);

/*
 * @doc: 
 * return non-uniform random number
 * according to distribution in dist[0..size-1]
 */

long LXDistribIRandom(float *dist, size_t size);

#endif



