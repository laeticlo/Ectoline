/* -----------------------------------------------------------------------
 * $Id: PMSpectrum.h 1935 2013-07-25 12:38:55Z viari $
 * -----------------------------------------------------------------------
 * @file: PMSpectrum.h
 * @desc: PMSpectrum library
 *
 * @history:
 * @+ <Romain> : Jan 03 : first version
 * @+ <Wanou>  : Jan 03 : minor modifications
 * @+ <Gloup>  : Dec 05 : major revision
 * @+ <Gloup>  : Feb 06 : code cleanup
 * @+ <Gloup>  : Aug 06 : moved from TG to PM library
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PMSpectrum library<br>
 * Defines spectrum structure & associated i/o functions,<br> 
 *
 * @docend:
 */

#ifndef _H_PMSpectrum
#define _H_PMSpectrum

#include "LX/LXSys.h"

#include "PMSys.h"

#include "PMAa.h"

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */
/*
 * @doc: minimum spectrum capacity(number of peaks)
 */
#define PM_MIN_SPECTRUM_CAPACITY 256

/*
 * @doc: maximum length for spectrum id name
 */
#define PM_SPECT_ID_LEN 1024

/*
 * @doc: default mass resolution
 */
#define PM_DFT_PPM 100.

/*
 * @doc: known spectrum formats
 */
 
#define PM_PKL1_FORMAT "pkl"  /* standard pkl : parent line is [M+zH]/z intensity z */
#define PM_PKL2_FORMAT "PKL"  /* modified pkl : parent line is [M+H] intensity z    */
#define PM_DTA1_FORMAT "dta"  /* standard dta : parent line is [M+H] intensity      */
#define PM_DTA2_FORMAT "DTA"  /* standard dta : parent line is [M+H] intensity      */

/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/*
 * @doc: PMPeak structure
 * - valid : int - *internal* flag used by Taggor 
 * - rank  : int - peak rank 
 * - mass : float - peak mass
 * - intensity : float - peak intensity
 * - proba : float - peak probability = intensity / SUM(intensity)
*/
typedef struct {
    int    valid;
    int    rank;
    float  mass;
    float  intensity;
    float  proba;
} PMPeak;

/*
 * @doc: PMSpectrum structure
 * - id    : char[PM_SPECT_ID_LEN] - spectrum id
 * - capacity : int - current peak capacity *internal*
 * - nbPeaks : int - number of peaks in spectrum
 * - charge: int - spectrum parent charge
 * - dmm : float - spectrum resolution (mass tolerance)
 * - parent : PMPeak - peak parent - note that parent->mass corresponds to the [MH]+ ion
 * - peak : * PMPeak - peak pointer
 */

typedef struct {
  int     capacity;
  int     nbPeaks;
  int     charge;
  float   dmm;
  PMPeak  parent;
  PMPeak  *peak;
  char    id[PM_SPECT_ID_LEN + 2];
} PMSpectrum;

/*
 * @doc: PMSpectrum reader
 */

typedef int (*PMSpectrumReader) (FILE *streamin, PMSpectrum *sp);
typedef int (*PMSpectrumWriter) (FILE *streamin, PMSpectrum *sp);

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * Allocate a new (empty) PMSpectrum
 * @return PMSpectrum* 
 */
PMSpectrum *PMNewSpectrum (void);

/*
 * @doc: 
 * Allocate a new (empty) PMSpectrum and copy header information
 * from given spectrum. the peaks are not copied.
 * @param PMSpectrum *sp (PMSpectrum to get information from)  
 * @return new PMSpectrum* 
 */
PMSpectrum *PMNewShallowSpectrum(PMSpectrum *sp);

/*
 * @doc: 
 * Allocate a new (empty) PMSpectrum and copy everything
 * from given spectrum. the peaks are copied.
 * @param PMSpectrum *sp (PMSpectrum to copy)  
 * @return new PMSpectrum* 
 */
PMSpectrum *PMCopySpectrum(PMSpectrum *sp);

/*
 * @doc: 
 * Resize PMSpectrum memory allocated according to given capacity
 * @param PMSpectrum *sp (PMSpectrum to resize)  
 * @param int capacity (new maximum number of peaks)
 * @return PMSpectrum* 
 */

PMSpectrum *PMResizeSpectrum (PMSpectrum *sp, int capacity);

/*
 * @doc:
 * Add given peak to PMSpectrum
 * @param PMSpectrum *sp (PMSpectrum to add peak to)
 * @param PMPeak peak (peak to be added)
 * @return PMSpectrum*
 */
PMSpectrum *PMAddPeakToSpectrum (PMSpectrum *sp, PMPeak peak );

/*
 * @doc: 
 * Free a previously allocated PMSpectrum structure.
 * @param PMSpectrum *sp (PMSpectrum to free)
 * @return NULL
 */

PMSpectrum *PMFreeSpectrum (PMSpectrum *sp);

/*
 * @doc:
 * Assign peaks probabilities to intensity / Sum(intensities)
 * @param PMSpectrum *sp (PMSpectrum to handle)
 * @return LX_NO_ERROR
 */

int PMProbabilizeSpectrum(PMSpectrum *sp);

/*
 * @doc:
 * Assign peaks ranks based on intensities
 * @param PMSpectrum *sp (PMSpectrum to handle)
 * @return LX_NO_ERROR
 */

int PMRankizeSpectrum(PMSpectrum *sp);

/*
 * @doc:
 * Filter spectrum on masses. keep only peaks within mass range [massMin, massMax]
 * @param PMSpectrum *sp (PMSpectrum to filter)
 * @param float massMin (minimum mass)
 * @param float massMax (maximum mass)
 * @return New (filtered) spectrum
 */

PMSpectrum *PMFilterMassSpectrum(PMSpectrum *sp, float massMin, float massMax);

/*
 * @doc:
 * Filter spectrum on probas. keep only peaks within proba range [probaMin, probaMax]
 * @param PMSpectrum *sp (PMSpectrum to filter)
 * @param float probaMin (minimum proba)
 * @param float probaMax (maximum proba)
 * @return New (filtered) spectrum
 */

PMSpectrum *PMFilterProbaSpectrum(PMSpectrum *sp, float probaMin, float probaMax);

/*
 * @doc:
 * Filter spectrum on ranks. keep only peaks within rank range [rankMin, rankMax]
 * @param PMSpectrum *sp (PMSpectrum to filter)
 * @param int rankMin (minimum rank)
 * @param int rankMax (maximum rank)
 * @return New (filtered) spectrum
 */

PMSpectrum *PMFilterRankSpectrum(PMSpectrum *sp, int rankMin, int rankMax);

/*
 * @doc:
 * Filter spectrum on aminoacid doublets
 * keep only peaks that have a mass difference
 * of 1 aa with at least another peak
 *
 * this will speedup the search without
 * changing the results.
 * @param PMSpectrum *sp (PMSpectrum to filter)
 * @param PMAminoAlphabet *AaDic (aminoacid alphabet)
 * @return New (filtered) spectrum
 */

PMSpectrum *PMFilterDoubletSpectrum(PMSpectrum *sp, PMAminoAlphabet *AaDic);

/*
 * @doc:
 * Recalibrate spectrum, shifting all peaks by dm/m
 *
 * @param PMSpectrum *sp (PMSpectrum to recalibrate)
 * @param dmm float quantity to shift peaks
 * @return New (recalibrated) spectrum
 */

PMSpectrum *PMRecalibrateSpectrum(PMSpectrum *sp, float dmm);

/*
 * @doc: 
 * Read a PMSpectrum from file (DTA format).
 * @param streamin FILE* to read in
 * @param PMSpectrum *sp PMSpectrum to read
 * @return error code depending on read issue
 * LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */
int PMReadDTA1Spectrum (FILE *streamin, PMSpectrum *sp);

/*
 * @doc: 
 * Read a PMSpectrum from file (standard PKL format).
 * parent ion line is [M+zH]/z intensity z
 * @param streamin FILE* to read in
 * @param PMSpectrum *sp PMSpectrum to read
 * @return error code depending on read issue
 * LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */
int PMReadPKL1Spectrum (FILE *streamin, PMSpectrum *sp);

/*
 * @doc: 
 * Read a PMSpectrum from file (modified PKL format).
 * parent ion line is [M+H] intensity z
 * @param streamin FILE* to read in
 * @param PMSpectrum *sp PMSpectrum to read
 * @return error code depending on read issue
 * LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */
int PMReadPKL2Spectrum (FILE *streamin, PMSpectrum *sp);

/*
 * @doc: 
 * Write a PMSpectrum to a file (DTA format)
 * @param streamou File* to write in
 * @param PMSpectrum *sp PMSpectrum to write
 * @return error code depending on write issue
 * LX_NO_ERROR, LX_IO_ERROR
 */
int PMWriteDTA1Spectrum (FILE *streamou, PMSpectrum *sp);

/*
 * @doc: 
 * Write a PMSpectrum to a file (standard PKL format)
 * parent ion line is [M+zH]/z intensity z
 * @param streamou File* to write in
 * @param PMSpectrum *sp PMSpectrum to write
 * @return error code depending on write issue
 * LX_NO_ERROR, LX_IO_ERROR
 */
int PMWritePKL1Spectrum (FILE *streamou, PMSpectrum *sp);

/*
 * @doc: 
 * Write a PMSpectrum to a file (modified PKL format)
 * parent ion line is [M+H] intensity z
 * @param streamou File* to write in
 * @param PMSpectrum *sp PMSpectrum to write
 * @return error code depending on write issue
 * LX_NO_ERROR, LX_IO_ERROR
 */
int PMWritePKL2Spectrum (FILE *streamou, PMSpectrum *sp);

/*
 * @doc: 
 * get a PMSpectrumReader according to format
 * @param format reader format (PKL, pkl, DTA)
 * @return PMSpectrumReader or NULL if unkknown format
 */

PMSpectrumReader PMGetSpectrumReader(char *format);

/*
 * @doc: 
 * get a PMSpectrumWriter according to format
 * @param format writer format (PKL, pkl, DTA)
 * @return PMSpectrumWriter or NULL if unkknown format
 */

PMSpectrumWriter PMGetSpectrumWriter(char *format);

#endif
