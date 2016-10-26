/* -----------------------------------------------------------------------
 * $Id: PMClust.h 1112 2010-05-23 18:01:09Z viari $
 * -----------------------------------------------------------------------
 * @file: PMClust.h
 * @desc: PMClust library
 *
 * @history:
 * @+ <Wanou> : Jan 02 : first version
 * @+ <Gloup> : Aug 06 : complete code revision
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PMClust library <br>
 *
 * @docend:
 */

#ifndef _H_PMClust
#define _H_PMClust

#ifndef _H_LX
#include "LX.h"
#endif

#ifndef _H_PM
#include "PM.h"
#endif

/* ----------------------------------------------- */
/* constants                                       */
/* ----------------------------------------------- */

/* 
 * @doc: Default parameter values
 */

#define DFT_DISTANCE           3000
#define DFT_MIN_FULL           3
#define DFT_MIN_PEPHITS        1

/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/*
 * @doc: Hit with additional information
 */

typedef struct {
   long          from;     /* left nuc. position  */
   long          to;       /* right nuc. position */
   PMSeqId       *seqId;
   PMTag         *tag;
   PMHit         *hit;
} ExtendedHit;

/*
 * @doc: cluster
 */

typedef struct {
   int        nbHits;
   int        nbFull;
   int        nbPephits;
   float      score;
   LXStackp   *xhits;      /* stack of extended hits  */
} Cluster;

/*
 * @doc: Parameters structure
 */

typedef struct {
  int   distance;
  int   minFull;
  int   minPephits;
  Bool  isProteic;
  Bool  framed;
  Bool  addPartial;
  Bool  verbose;
} Parameter;

/* ----------------------------------------------- */
/* prototypes                                      */
/* ----------------------------------------------- */

                /* ------------------------------------ */
                /* PMClustHelp.c                        */
                /* ------------------------------------ */

void StartupBanner(int argc, char *argv[]);

void ExitUsage(int stat);

void PrintHelp(void);

                /* ------------------------------------ */
                /* PMClustUtil.c						*/
                /* ------------------------------------ */

int         UtilCompareSeqId(const void *p1, const void *p2);

void        UtilFreeSeqId(void *p);

int         UtilCompareTag(const void *p1, const void *p2);

void        UtilFreeTag(void *p);

int         UtilReadExtendedHits(LXStackp *stkp, LXDict *seqDict,
                                 LXDict *tagDict, Parameter *param);

ExtendedHit *UtilNewXHit(PMSeqId *seqId, PMTag *tag , PMHit *hit, Bool isProteic);

void        UtilFreeXHit(void *data);


#endif
