/* -----------------------------------------------------------------------
 * $Id: Taggor.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: Taggor.h
 * @desc: Taggor library
 *
 * @history:
 * @+ <romain>  : Feb 03 : first version
 * @+ <Gloup>   : Dec 05 : major revision
 * @+ <Gloup>   : Feb 06 : code cleanup
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * Taggor include file<br>
 *
 * @docend:
 */

#ifndef _H_Taggor
#define _H_Taggor

#include "LX.h"
#include "PM.h"

/* ---------------------------------------------------- */
/* constantes                                           */
/* ---------------------------------------------------- */

/* 
 * @doc: Default parameter values
 */

#define DFT_OLIGO_LENGTH      3
#define DFT_NB_RES            10
#define DFT_TOLERANCE         50
#define DFT_PEAK_THRESHOLD    0.1
#define DFT_RANK_THRESHOLD    50
#define DFT_MASS_THRESHOLD    0.0
#define DFT_STAT_EST_PARAM    0
#define DFT_STAT_R_PARAM      10
#define DFT_MIN_SCORE         0
#define DFT_DENOVO_RMIN       1.0
#define DFT_AA_FILENAME       "aa_mono.ref"

/* ---------------------------------------------------- */
/* data structures                                      */
/* ---------------------------------------------------- */

/*
 * @doc: Types of tags statistics
 */

typedef enum {
    UnknownStat = 0,
    RankStat,
    EmpiricalStat,
    SpectrumStat,
    TheoreticalStat
} StatMode;

/*
 * @doc: Types of tags unicity
 */

typedef enum {
    NoUnique = 0,
    UniqueSeq,
    UniqueTag,
} UniqueMode;

/*
 * @doc: Parameters structure
 */

typedef struct {
                                  /* -- Users parameters --               */
  int        oligoLength;         /* PST tag lengh                        */
  int        nbResults;           /* output max number of results         */
  int        statESTParam;        /* stat. E.S.T parameter                */
  int        statRParam;          /* stat. R parameter                    */
  int        rankThreshold;       /* spectrum rank threshold              */
  float      peakThreshold;       /* spectrum peak intensity threshold    */
  float      massThreshold;       /* spectrum mass threshold              */
  float      massTolerance;       /* mass tolerance                       */
  float      minScore;            /* output min score                     */
  float      denovoRmin;          /* rmin denovo parameter                */
  Bool       verbose;             /* verbose option                       */
  Bool       recalibrate;         /* recalibrate spectra                  */
  Bool       denovo;              /* denovo sequencing                    */
  Bool       tagCorrelation;      /* tag correlation option               */
  StatMode   statMode;            /* stat mode choice                     */
  UniqueMode uniqueMode;          /* unique mode choice                   */
} Parameter;

/* ---------------------------------------------------- */
/* prototypes                                           */
/* ---------------------------------------------------- */

                /* ------------------------------------ */
                /* TaggorHelp.c                         */
                /* ------------------------------------ */

/*
 *@doc:
 *print program parameters
 *@param param Parameter* prog. parameter
 */
void PrintParameters(Parameter *param);

/*
 *@doc:
 *print startup banner
 *@param argc main arg count
 *@param argv main argv
 */
 
void StartupBanner(int argc, char *argv[]);

/*
 *@doc:
 *print usage and exit(stat)
 *@param stat exit status 
 */
 
void ExitUsage(int stat);

/*
 *@doc:
 *print Help
 */

void PrintHelp(void);

                /* ------------------------------------ */
                /* TaggorAlgo.c                         */
                /* ------------------------------------ */

/*
 *@doc:
 *Filter Spectrum (create a new spectrum with all peak probability > pmin)
 *@param sp Spectrum* to filter
 *@param alpha PMAminoAlphabet* Amino Acid alphabet
 *@param param Parameter* user's parameter
 *@return Spectrum* filtered
 */

PMSpectrum *PreprocessSpectrum(PMSpectrum *sp, PMAminoAlphabet *alpha, Parameter *param);

/*
 *@doc:
 *@param sp Spectrum* to process
 *@param alpha PMAminoAlphabet* Amino Acid alphabet
 *@param param Parameter* user's parameter
 *@param dmmStat LXStat* statistical structure to keep dm/m stats (may be NULL)
 */
 
PMTagStack *ComputeTags(PMSpectrum *sp, PMAminoAlphabet *alpha, Parameter *param, LXStat *dmmStat);

#endif
