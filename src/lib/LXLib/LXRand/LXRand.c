/* -----------------------------------------------------------------------
 * $Id: LXRand.c 1782 2013-02-23 00:11:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXRand.c
 * @desc: random numbers utilities. 
 * 
 * @history:
 * @+ <Gloup> : Aug 06 : added to LXlib 
 * @+ <Gloup> : Jan 10 : random generators added 
 * -----------------------------------------------------------------------
 * @docstart:
 *
 *  random numbers utility library<br>
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include "LX/LXSys.h"
#include "LX/LXMacros.h"
#include "LX/LXRand.h"

#define EPS     DBL_MIN
#define RNMX    (1.0 - DBL_MIN)

#define IA_PM	16807
#define IM_PM	2147483647
#define AM_PM	(1.0/IM_PM)
#define IQ_PM	127773
#define IR_PM	2836
#define NTAB_PM	32
#define NDIV_PM	(1+(IM_PM-1)/NTAB_PM)

#define IM1_LE	2147483563
#define IM2_LE	2147483399
#define AM_LE	(1.0/IM1_LE)
#define IMM1_LE	(IM1_LE-1)
#define IA1_LE	40014
#define IA2_LE	40692
#define IQ1_LE	53668
#define IQ2_LE	52774
#define IR1_LE	12211
#define IR2_LE	3791
#define NTAB_LE	32
#define NDIV_LE (1+IMM1_LE/NTAB_LE)

#define MBIG_KN	    1000000000L
#define MSEED_KN    161803398L
#define NTAB_KN     55
#define MZ_KN	    0
#define	FAC_KN	    (1.0/MBIG_KN)


#ifdef LX_OS_WIN32
// [AV] -> [Jet] : please check if there is no 'random' function
// in MingW. On Unix these functions are not equivalent at all.
// rand/srand is the bad guy :-)
#define random   rand
#define srandom  srand
#endif

/* ---------------------------------------------------- */
/* internal types                                       */
/* ---------------------------------------------------- */

typedef void   (*SeedFunc) (unsigned seed);
typedef double (*RandFunc) (void);

typedef struct {
    UInt64  A;
    UInt64  C;
    UInt64  M;
    UInt64  Mask;
    int     Shift;
} LCG_Param;

/* ---------------------------------------------------- */
/* internal variables                                   */
/* ---------------------------------------------------- */

static long sSeedPM = 0;
static long sSeedLE = 0;

static long sIyPM = 0;
static long sIvPM[NTAB_PM];

static long sSeed2LE = 0;
static long sIyLE = 0;
static long sIvLE[NTAB_LE];

static int  sInextKN;
static int  sInextpKN;
static int  sIffKN = 0;
static long sMaKN[NTAB_KN+1];

static UInt64 sSeedLC = 0;

static LCG_Param sLCGParamNumRec = {
    1664525LLU, 1013904223LLU, 0x100000000LLU,
    0x0, 0
};

static LCG_Param sLCGParamCarbon = {
    16807LLU, 0LLU, 0x7fffffffLLU,
    0x0, 0
};

static LCG_Param sLCGParamJava = {
    25214903917LLU, 11LLU, 0x1000000000000LLU,
    0x1fffffffd0000LLU, 16
};


static void sSeedUnix(unsigned seed);
static double sRandUnix(void);

static SeedFunc sCurrentSeedFunc = sSeedUnix;
static RandFunc sCurrentRandFunc = sRandUnix;

static LCG_Param *sCurrentLCGParam = &sLCGParamNumRec;

static Bool sInit = FALSE;


/* ---------------------------------------------------- */
/* Internal generators                                  */
/* sSeedXXX : initialize generator from seed            */
/* sRandXXX : generate a uniform random double in range */
/*  ]0., 1.[   (i.e. exclusive of the endpoint values   */ 
/* ---------------------------------------------------- */

/* ---------------------------------------------------- */
/* Unix                                                 */

static void sSeedUnix(unsigned seed) {
    srandom(seed);
}

static double sRandUnix(void) {
    double temp =  random() / (double) LONG_MAX;
    return ((temp < EPS) ? EPS : ((temp > RNMX) ? RNMX : temp));
}

/* ---------------------------------------------------- */
/* Park-Miller                                          */

static void sSeedParkMiller(unsigned seed) {
    int  i;
    long k, kseed;
    
    kseed = (seed == 0 ? 1 : seed);    /* 0 not allowed */

    for (i = NTAB_PM + 7 ; i >= 0 ; i--){
        k = kseed / IQ_PM;
        kseed = IA_PM * (kseed - k * IQ_PM) - IR_PM * k;
        if (kseed < 0)
            kseed += IM_PM;
        if (i < NTAB_PM)
            sIvPM[i] = kseed;
    }
    
    sIyPM = sIvPM[0];
    
    sSeedPM = kseed;
}

static double sRandParkMiller(void) {
    int   i;
    long  k;
    
    k = sSeedPM / IQ_PM;
    
    sSeedPM = IA_PM * (sSeedPM - k * IQ_PM) - IR_PM * k;    /* IA*s%IM */

    if (sSeedPM < 0)
	sSeedPM += IM_PM;

    i = sIyPM / NDIV_PM;

    sIyPM = sIvPM[i];  
    
    sIvPM[i] = sSeedPM;
    
    return (AM_PM * sIyPM);
}

/* ---------------------------------------------------- */
/* LECUYER                                              */

static void sSeedLecuyer(unsigned seed) {
    int  i;
    long k, kseed;
    
    kseed = (seed == 0 ? 1 : seed);    /* 0 not allowed */
        
    for ( i = NTAB_LE + 7 ; i >= 0 ; i--){
        k = kseed / IQ1_LE;
        kseed = IA1_LE * (kseed - k * IQ1_LE) - IR1_LE * k;
        if (kseed < 0)
            kseed += IM1_LE;
        if (i < NTAB_LE)
            sIvLE[i] = kseed;
    }
    
    sIyLE = sIvLE[0];
    
    sSeedLE = kseed;
    
    sSeed2LE = 123456789;
}

static double sRandLecuyer(void) {
    int   i;
    long  k;
    
    k = sSeedLE / IQ1_LE;
    
    sSeedLE = IA1_LE * (sSeedLE - k * IQ1_LE) - IR1_LE * k;    /* IA*s%IM */

    if (sSeedLE < 0)
	sSeedLE += IM1_LE;

    k = sSeed2LE / IQ2_LE;
    
    sSeed2LE = IA2_LE * (sSeed2LE - k * IQ2_LE) - IR2_LE * k;    /* IA*s%IM */

    if (sSeed2LE < 0)
	sSeed2LE += IM2_LE;

    i = sIyLE / NDIV_LE;

    sIyLE = sIvLE[i] - sSeed2LE;  
    
    sIvLE[i] = sSeedLE;
    
    if (sIyLE < 1)
	sIyLE += IMM1_LE;
	
    return (AM_LE * sIyLE);
}

/* ---------------------------------------------------- */
/* KNUTH                                                */

static void sSeedKnuth(unsigned seed) {
    int  i;
    long mj, mk;

    mj = (MSEED_KN - seed) % MBIG_KN;

    sMaKN[NTAB_KN] = mj;
    
    mk = 1;
    
    for (i = 0 ; i < NTAB_KN ; i++) {
	    int ii = (21*i) % NTAB_KN;
	    sMaKN[ii] = mk;
	    mk = mj - mk;
	    if (mk < MZ_KN)
	        mk += MBIG_KN;
	    mj = sMaKN[ii];
    }
    
    for (i = 1 ; i <= 4 ; i++) {
        int j;
	    for (j = 1 ; j <= NTAB_KN ; j++) {
	        sMaKN[j] -= sMaKN[1+(j+30)%NTAB_KN];
	        if (sMaKN[j] < MZ_KN)
	            sMaKN[j] += MBIG_KN; 
	    }
    }

    sInextKN  = 0;
    sInextpKN = 31;

    sIffKN = 1;
}

static double sRandKnuth(void) {
    long mj;

    if (++sInextKN > NTAB_KN)
	    sInextKN = 1;
	
    if (++sInextpKN > NTAB_KN)
	    sInextpKN = 1;

    mj = sMaKN[sInextKN] - sMaKN[sInextpKN];
    
    if (mj < MZ_KN)
	    mj += MBIG_KN;

    sMaKN[sInextKN] = mj;
    
    return (mj * FAC_KN);
}

/* ---------------------------------------------------- */
/* generic linear congruential                          */

static void sSeedLCG(unsigned seed) {
    sSeedLC = seed;
}

static double sRandLCG(void) {
  UInt64 utemp, norm;
  utemp = sSeedLC = (sCurrentLCGParam->A * sSeedLC + sCurrentLCGParam->C) % sCurrentLCGParam->M;
  norm  = sCurrentLCGParam->M;
  if (sCurrentLCGParam->Mask) {
    utemp = (utemp & sCurrentLCGParam->Mask) >> sCurrentLCGParam->Shift;
    norm  = (norm  & sCurrentLCGParam->Mask) >> sCurrentLCGParam->Shift;
  }
  double temp = (double) utemp / (double) norm;
  return ((temp < EPS) ? EPS : ((temp > RNMX) ? RNMX : temp));
}

/* ---------------------------------------------------- */
/* API                                                  */
/* ---------------------------------------------------- */

/* ---------------------------------------------------- */
/* select a random genrator                             */
/* ---------------------------------------------------- */

void LXSelectRandom(LXRandomSelector selector) {

    switch (selector) {

        case LX_RAND_UNIX:
            sCurrentSeedFunc = sSeedUnix;
            sCurrentRandFunc = sRandUnix;
            break;

        case LX_RAND_PARK_MILLER_BD:
            sCurrentSeedFunc = sSeedParkMiller;
            sCurrentRandFunc = sRandParkMiller;
            break;

        case LX_RAND_LECUYER_BD:
            sCurrentSeedFunc = sSeedLecuyer;
            sCurrentRandFunc = sRandLecuyer;
            break;

        case LX_RAND_KNUTH:
            sCurrentSeedFunc = sSeedKnuth;
            sCurrentRandFunc = sRandKnuth;
            break;

        case LX_RAND_LCG_NUMREC:
            sCurrentSeedFunc = sSeedLCG;
            sCurrentRandFunc = sRandLCG;
            sCurrentLCGParam = &sLCGParamNumRec;
            break;

        case LX_RAND_LCG_CARBON:
            sCurrentSeedFunc = sSeedLCG;
            sCurrentRandFunc = sRandLCG;
            sCurrentLCGParam = &sLCGParamCarbon;
            break;

        case LX_RAND_LCG_JAVA:
            sCurrentSeedFunc = sSeedLCG;
            sCurrentRandFunc = sRandLCG;
            sCurrentLCGParam = &sLCGParamJava;
            break;

        default:
            sCurrentSeedFunc = sSeedUnix;
            sCurrentRandFunc = sRandUnix;
    }

    sInit = FALSE;
}

/* ---------------------------------------------------- */
/* initialize the random number generator with seed     */
/* if seed == 0 : use time to make the seed             */
/* ---------------------------------------------------- */
void LXInitRandom(unsigned seed)
{
    if (seed == 0)
      seed = time(0) & 0xfffffff;

    sCurrentSeedFunc(seed);

    sInit = TRUE;
}

/* ---------------------------------------------------- */
/* returns a uniform random deviate between 0. and 1.   */
/* (exclusive of the endpoint values, that is in range  */
/* [DBL_MIN, 1-DBL_MIN]                                 */
/* ---------------------------------------------------- */
double LXUniformFRandom()
{
    double temp;

    if (! sInit)
       LXInitRandom(0);
  
    temp = sCurrentRandFunc();

    return ((temp < EPS) ? EPS : ((temp > RNMX) ? RNMX : temp));
}

/* ---------------------------------------------------- */
/* return uniform random long in [from, to[             */
/* ---------------------------------------------------- */
long LXUniformIRandom(long from, long to)
{
    long i;
    
                                /* will truncate to integer part    */
                                /* in [0, (to-from)[                */

    i  = LXUniformFRandom () * (to - from);       

    i += from;                  /* shift origin                     */

    if (i >= to)                /* very unlikely but == may appear  */
       i--;                     /* from truncation                  */
    
    return i;
}

/* ---------------------------------------------------- */
/* return non-uniform random number                     */
/* according to distribution in dist[0..size-1]         */
/* ---------------------------------------------------- */
long LXDistribIRandom(float *dist, size_t size)
{
    size_t i;
    double sum, unif;
    
                /* draw unif ]0., 1.[                   */

    unif = LXUniformFRandom();

                /* search on repartition function       */

    for (i = 0, sum = 0.; i < size ; i++) {
        sum += dist[i];
        if (unif <= sum)
            return i;
    }
    
    return (size-1);
}

