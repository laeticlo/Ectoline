/* -----------------------------------------------------------------------
 * $Id: PMMatch.h 993 2010-04-14 22:51:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMMatch.h
 * @desc: PMMatch library
 *
 * @history:
 * @+ <Wanou> : Jan 02 : first version
 * @+ <Gloup> : Aug 06 : complete code revision
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PMMatch library <br>
 *
 * @docend:
 */

#ifndef _H_PMMatch
#define _H_PMMatch

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

#define DFT_TOLERANCE          50
#define DFT_MAX_MISCLEAVAGE    1
#define DFT_MAX_OVERCLEAVAGE   0
#define DFT_MAX_MODIF          0
#define DFT_MET_SCAN		   1
#define DFT_AA_FILENAME       "aa_mono.ref"
#define DFT_ENZ_FILENAME      "trypsine.ref"
#define DFT_HITSCORE           PSTHitScoreType


/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/*
 * @doc: Types of hit scores
 */

typedef enum {
    UnknownHitScoreType = 0,
    PSTHitScoreType,
    REMTHitScoreType,
    Dancink99HitScoreType,
    RankSumHitScoreType,
} HitScoreType;


/*
 * HashTable for hashing kuple
 */
 
typedef struct {
   Int32       kuple;
   Int32       size;
   LXStackpPtr *stk;
} HashTable;

/*
 * Count estimator (for stats)
 */
 
typedef struct {
   float       nTerm;
   float       cTerm;
   float       complete;
} CountEstimator;

/*
 * @doc: Parameters structure
 */

typedef struct {
  int   	    maxMisCleavage;
  int   	    maxOverCleavage;
  int   	    maxModif;
  int   	    metScan;
  float 	    massTolerance;
  Bool  	    differentiateKQ;
  Bool  	    partialHit;
  Bool  	    verbose;
  HitScoreType  hitScoreType;
} Parameter;


/* ----------------------------------------------- */
/* prototypes                                      */
/* ----------------------------------------------- */

                /* ------------------------------------ */
                /* PMMatchHelp.c                        */
                /* ------------------------------------ */

void StartupBanner(int argc, char *argv[]);

void ExitUsage(int stat);

void PrintHelp(void);

                /* ------------------------------------ */
                /* PMMatchHash.c                        */
                /* ------------------------------------ */

Int32      HashUple            (char *s, int kuple, Bool differentiateKQ);

Int32      HashNextUple        (char *s, int kuple, Bool differentiateKQ, Int32 previous);

HashTable *HashNewTable        (int kuple);

HashTable *HashFreeTable       (HashTable *table);

void       HashResetTable      (HashTable *table);

void       HashFlattenTable    (HashTable *table);

void       HashInitTable       (HashTable *table, PMTagStack *tagStack, Bool differentiateKQ);

void       HashFillTable       (HashTable *table, char *seq, int kuple, Bool differentiateKQ);

PMHitStack *HashLookupHits     (HashTable *table, PMTag *tag, Bool differentiateKQ);


                /* ------------------------------------ */
                /* PMMatchMass.c                        */
                /* ------------------------------------ */

Bool MassMatchHit (PMTag *tag, 
	 	           PMHit *hit,
		           LXFastaSequence *fastaSeq,
		           PMAminoAlphabet *alpha,
		           PMEnzyme *enzyme,
		           Parameter *param);

int MassMatchHitStack (PMTag *tag, 
		               PMHitStack *hitStack,
		               LXFastaSequence *fastaSeq,
		               PMAminoAlphabet *alpha,
		               PMEnzyme *enzyme,
		               Parameter *param);

                /* ------------------------------------ */
                /* PMMatchScore.c						*/
                /* ------------------------------------ */

int ScoreHitStack (PMTag *tag, 
                   PMHitStack *hitStack,
                   LXFastaSequence *fastaSeq,
                   PMAminoAlphabet *alpha,
                   PMEnzyme *enzyme,
                   Parameter *param);


                /* ------------------------------------ */
                /* PMMatchUtil.c						*/
                /* ------------------------------------ */

char *UtilValidateSequence (char *seq);

int   UtilCheckTags        (PMTagStack *tagStack);

int   UtilPrintHitStack    (PMTag *tag, PMHitStack *hitStack, LXFastaSequence *fastaSeq);


#endif
