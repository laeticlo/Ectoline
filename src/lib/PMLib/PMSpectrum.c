/* -----------------------------------------------------------------------
 * $Id: PMSpectrum.c 1810 2013-02-25 10:04:43Z viari $
 * -----------------------------------------------------------------------
 * @file: PMSpectrum.c
 * @desc: PMSpectrum library
 * -----------------------------------------------------------------------
 *
 * @history:
 * @+ <Romain> : Jan 03 : first version
 * @+ <Wanou>  : Jan 03 : Minor modification
 * @+ <Gloup>  : Aug 04 : several bugs corrections
 * @+ <Gloup>  : Dec 05 : major revision
 * @+ <Gloup>  : Feb 06 : code cleanup
 * @+ <Gloup>  : Aug 06 : moved from TG to PM library
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PMSpectrum i/o functions <br>
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXSys.h"

#include "PM.h"

/* ---------------------------------------------------- */
/* Statics                                              */
/* ---------------------------------------------------- */

static PMSpectrum *sCompareSpectrum; /* for peaks comparison */

/**
 * read parent ion from DTA
 * 1rst line (parent) is: 
 *   [M+H] intensity
 */
 
static int sReadDTAParent(FILE *streamin, PMPeak *peak)
{
  char buffer[BUFSIZ];

  if (! LXGetLine(buffer, sizeof(buffer), streamin))
    return LX_EOF;

  if (sscanf(buffer, "%f%f", &peak->mass, &peak->intensity) != 2) {
    return FIO_ERROR("Bad DTA parent");
  }
  
  return LX_NO_ERROR;
}

/**
 * read parent ion from PKL
 * 1rst line (parent) is: [M+zH]/z intensity z  // mzCorrection = TRUE
 * 1rst line (parent) is: [M+H] intensity z     // mzCorrection = FALSE
 */

static int sReadPKLParent(FILE *streamin, Bool mzCorrection, PMPeak *peak, int *charge)
{
  char buffer[BUFSIZ];

  if (! LXGetLine(buffer, sizeof(buffer), streamin))
    return LX_EOF;

  if (sscanf(buffer, "%f%f%d", &peak->mass, &peak->intensity, charge) != 3) {
    return FIO_ERROR("Bad PKL parent");
  }
  
  /* correct to actual mass of [M+H]+ */
  
  if (mzCorrection)
    peak->mass = (peak->mass * (float) *charge) - ((float) (*charge - 1) * PM_MASS_H);
  
  return LX_NO_ERROR;
}

/**
 * read all other ions from DTA/PKL
 */

static int sReadDTAPeaks(FILE *streamin, PMSpectrum *sp)
{
  char buffer[BUFSIZ];
  PMPeak peak;
  size_t nreads = 0;
  
  peak.valid = 1;   /* default peak values */
  peak.rank  = 0;
  peak.proba = -1;

  while (LXGetRawLine(buffer, sizeof(buffer), streamin)) {

    if (strlen(buffer) <= 2)
      break; // end of spectrum is usually an empty line 
             // this is a bad mark, but dta/pkl are like this :-(
      
    if (sscanf(buffer, "%f%f", &(peak.mass), &(peak.intensity)) != 2) {
      return FIO_ERROR("Bad peak format");
    }

    if (! PMAddPeakToSpectrum(sp, peak)) {
      return MEMORY_ERROR();
    }
    
    nreads++;
  }

  if (nreads == 0)
    return LX_EOF;

  return LX_NO_ERROR;
}

/**
 * Generic PKL/DTA peak reader
 */

static int sReadPKLDTAPeaks (FILE *streamin, PMSpectrum *sp)
{
  int res;
  
  /* Read Peaks */
  
  if ((res = sReadDTAPeaks(streamin, sp)) != LX_NO_ERROR) {
    IO_ERROR();
    return res;
  }

  (void) PMProbabilizeSpectrum(sp);
  
  (void) PMRankizeSpectrum(sp);
  
  return res;
}

/**
 * Generic DTA reader
 */

static int sReadDTASpectrum (FILE *streamin, PMSpectrum *sp)
{
  int res;
  
  /* Read Parent Ion */
  
  sp->charge = 1;
  
  if ((res = sReadDTAParent(streamin, &(sp->parent))) != LX_NO_ERROR) {
    return res;
  }
  
  /* Read Other Peaks */
  
  if ((res = sReadPKLDTAPeaks(streamin, sp)) != LX_NO_ERROR) {
    return res;
  }

  return res;
}

/**
 * Generic PKL reader
 */

static int sReadPKLSpectrum(FILE *streamin, Bool mzCorrection, PMSpectrum *sp)
{
  int res;
  
  /* Read Parent Ion */
  
  if ((res = sReadPKLParent(streamin, mzCorrection, 
                  &(sp->parent), &(sp->charge))) != LX_NO_ERROR) {
    return res;
  }
  
  /* Read Other Peaks */
  
  if ((res = sReadPKLDTAPeaks(streamin, sp)) != LX_NO_ERROR) {
    return res;
  }

  return res;
}

/**
 * write ions from DTA/PKL
 */

static int sWriteDTAPeaks (FILE *streamou, PMSpectrum *sp) 
{
  int i;

  for (i = 0 ; i < sp->nbPeaks ; i++) {
    (void) fprintf(streamou,"%.4f %.4f\n", sp->peak[i].mass, sp->peak[i].intensity);
  }
  
  (void) fprintf(streamou,"\n");

  return LX_NO_ERROR;
}

/**
 * peaks comparison function
 */

static int sCompareIntensities(const void *i1, const void *i2) {
  float f1 = sCompareSpectrum->peak[*((const int *)i1)].intensity;
  float f2 = sCompareSpectrum->peak[*((const int *)i2)].intensity;
  return ((f1 < f2) ? 1 : (f1 > f2) ? -1 : 0);
}

/**
 * search a given mass in array
 * return 1 if it exist m0 such that |m0 - mass| < dm
 *
 * note: due to the small array size (~20), there is
 * no need to perform dichotomy here
 */

static int sFindMass(float mass, float dm, float *table, int siz)
{
  int i;

  for (i = 0 ; i < siz ; i++) {
    float delta = (table[i] - mass);
    if (delta < 0) delta = -delta;
    if (delta < dm)
      return 1;
  }
  
  return 0;
}


/* ==================================================== */
/* PMSpectrum API                                       */
/* ==================================================== */

/* -------------------------------------------- */
/* PMSpectrum  allocation : new empty spectrum  */
/* -------------------------------------------- */

PMSpectrum *PMNewSpectrum ()
{
  PMSpectrum *sp;
  
  if (! (sp = NEW(PMSpectrum))) {
    MEMORY_ERROR();
    return NULL;
  }

  if (! (sp->peak = NEWN(PMPeak, PM_MIN_SPECTRUM_CAPACITY))) {
    MEMORY_ERROR();
    return PMFreeSpectrum(sp);
  }

  sp->capacity = PM_MIN_SPECTRUM_CAPACITY;
  sp->nbPeaks  = 0;
  sp->charge   = 1.;
  sp->dmm      = PM_DFT_PPM * 1e-6;
  
  (void) strcpy(sp->id, "NO_ID");
  
  return sp;
}

/* -------------------------------------------- */
/* PMSpectrum  allocation : new empty spectrum  */
/* with information from another spectrum       */
/* -------------------------------------------- */

PMSpectrum *PMNewShallowSpectrum(PMSpectrum *sp)
{
  PMSpectrum *nsp;
  
  if (! (nsp = PMNewSpectrum())) {
    MEMORY_ERROR();
    return NULL;
  }

  nsp->parent  = sp->parent;
  nsp->dmm     = sp->dmm;
  nsp->charge  = sp->charge;

  (void) strcpy(nsp->id, sp->id);
  
  return nsp;
}

/* -------------------------------------------- */
/* Copy of a spectrum                           */
/* -------------------------------------------- */

PMSpectrum *PMCopySpectrum(PMSpectrum *sp)
{
  int i;
  PMSpectrum *nsp = PMNewShallowSpectrum(sp);
  
  for (i = 0 ; i < sp->nbPeaks ; i++) {
    if (! (nsp = PMAddPeakToSpectrum(nsp, sp->peak[i]))) {
      MEMORY_ERROR();
      return PMFreeSpectrum(nsp);
    }
  }

  return nsp;  
}


/* -------------------------------------------- */
/* PMSpectrum  deallocation                     */
/* -------------------------------------------- */

PMSpectrum *PMFreeSpectrum (PMSpectrum *sp) 
{

  if (sp) {
    if (sp->peak) {
      FREE(sp->peak);
    }
    FREE(sp);
  }

  return NULL;
}

/* -------------------------------------------- */
/* PMSpectrum Resizing                          */
/* -------------------------------------------- */

PMSpectrum *PMResizeSpectrum (PMSpectrum *sp, int capacity) 
{
  PMPeak *npeak;
  
  if (!sp || !sp->peak || (capacity < 0)) {
    ARG_ERROR();
    return NULL;
  }

  npeak = REALLOC(sp->peak, PMPeak, capacity);
  
  if (! npeak) {
    MEMORY_ERROR();
    return PMFreeSpectrum(sp);
  }
  
  sp->peak = npeak;
  sp->capacity = capacity;
  
  return sp; 
}

/* -------------------------------------------- */
/* Add PMPeak to PMSpectrum                     */
/* -------------------------------------------- */

PMSpectrum *PMAddPeakToSpectrum (PMSpectrum *sp, PMPeak peak)
{
  if (!(sp && sp->peak)) {
    ARG_ERROR();
    return NULL;
  }
  
  if (sp->nbPeaks >= sp->capacity) {
    PMSpectrum *nsp = PMResizeSpectrum(sp, (2 * sp->capacity) + 1);
    if (nsp == NULL) {
      MEMORY_ERROR();
      return PMFreeSpectrum(sp);
    }
    sp = nsp;
  }

  sp->peak[sp->nbPeaks] = peak;
  sp->nbPeaks++;
  
  return sp;
}

/* -------------------------------------------- */
/* compute spectrum peaks probabilities         */
/* -------------------------------------------- */

int PMProbabilizeSpectrum(PMSpectrum *sp)
{
  int i;
  float sum = 0.;
  
  if (!(sp && sp->peak)) {
    return ARG_ERROR();
  }

  if (sp->nbPeaks == 0) {
    return LX_WARNING("No peak in spectrum");
  }
  
  /* assign probabilities */
  
  for (i = 0 ; i < sp->nbPeaks ; i++)
    sum += sp->peak[i].intensity;
  
  if (sum <= 0.) {
    return LX_WARNING("Sum of intensities <= 0");
 }
      
  for (i = 0 ; i < sp->nbPeaks ; i++)
    sp->peak[i].proba = sp->peak[i].intensity / sum;

  return LX_NO_ERROR;
}

/* -------------------------------------------- */
/* compute peaks ranks                          */
/* -------------------------------------------- */

int PMRankizeSpectrum(PMSpectrum *sp)
{
  int i, *indexes;

  if (! (indexes = NEWN(int, sp->nbPeaks)))
    return MEMORY_ERROR();
    
  for (i = 0 ; i < sp->nbPeaks ; i++)
    indexes[i] = i;
  
  sCompareSpectrum = sp;  
  qsort(indexes, sp->nbPeaks, sizeof(int), sCompareIntensities);

  for (i = 0 ; i < sp->nbPeaks ; i++)
    sp->peak[indexes[i]].rank = i+1;
    
  FREE(indexes);

  return LX_NO_ERROR;
}

/* -------------------------------------------- */
/* filter a spectrum on mass                    */
/* -------------------------------------------- */

PMSpectrum *PMFilterMassSpectrum(PMSpectrum *sp, float massMin, float massMax)
{
  int i;
  PMSpectrum *nsp;
  
  if ((sp == NULL) || (sp->peak == NULL)) {
    ARG_ERROR();
    return NULL;
  }
  
  if ((massMin < 0.) || (massMin > massMax)) {
    ARG_ERROR();
    return NULL;
  }

  if (! (nsp = PMNewShallowSpectrum(sp))) {  
    MEMORY_ERROR();
    return NULL;
  }

  for (i = 0 ; i < sp->nbPeaks ; i++) {
    float mass = sp->peak[i].mass;
    if ((mass >= massMin) && (mass <= massMax)) {
      if (! (nsp = PMAddPeakToSpectrum(nsp, sp->peak[i]))) {
        MEMORY_ERROR();
        return PMFreeSpectrum(nsp);
      }
    }
  }
  
  return nsp;
}

/* -------------------------------------------- */
/* filter a spectrum on proba                   */
/* -------------------------------------------- */

PMSpectrum *PMFilterProbaSpectrum(PMSpectrum *sp, float probaMin, float probaMax)
{
  int i;
  PMSpectrum *nsp;
  
  if ((sp == NULL) || (sp->peak == NULL)) {
    ARG_ERROR();
    return NULL;
  }
  
  if (   (probaMin < 0.) || (probaMax > 1.) 
      || (probaMin > probaMax)) {
    ARG_ERROR();
    return NULL;
  }

  if (! (nsp = PMNewShallowSpectrum(sp))) {  
    MEMORY_ERROR();
    return NULL;
  }

  for (i = 0 ; i < sp->nbPeaks ; i++) {
    float proba = sp->peak[i].proba;
    if ((proba >= probaMin) && (proba <= probaMax)) {
      if (! (nsp = PMAddPeakToSpectrum(nsp, sp->peak[i]))) {
        MEMORY_ERROR();
        return PMFreeSpectrum(nsp);
      }
    }
  }
  
  return nsp;
}

/* -------------------------------------------- */
/* filter a spectrum on ranks                   */
/* -------------------------------------------- */

PMSpectrum *PMFilterRankSpectrum(PMSpectrum *sp, int rankMin, int rankMax)
{
  int i;
  PMSpectrum *nsp;
  
  if ((sp == NULL) || (sp->peak == NULL)) {
    ARG_ERROR();
    return NULL;
  }
  
  if ((rankMin < 0) || (rankMin > rankMax)) {
    ARG_ERROR();
    return NULL;
  }

  if (! (nsp = PMNewShallowSpectrum(sp))) {  
    MEMORY_ERROR();
    return NULL;
  }

  for (i = 0 ; i < sp->nbPeaks ; i++) {
    int rank = sp->peak[i].rank;
    if ((rank >= rankMin) && (rank <= rankMax)) {
      if (! (nsp = PMAddPeakToSpectrum(nsp, sp->peak[i]))) {
        MEMORY_ERROR();
        return PMFreeSpectrum(nsp);
      }
    }
  }
  
  return nsp;
}


/* -------------------------------------------- */
/* filter a spectrum on aminoacid doublets      */
/* keep only peaks that have a mass difference  */
/* of 1 aa with at least another peak           */
/*                                              */
/* this will speedup the search without         */
/* changing the results.                        */
/* -------------------------------------------- */

PMSpectrum *PMFilterDoubletSpectrum(PMSpectrum *sp, PMAminoAlphabet *AaDic)
{
  int i, j, naa;
  float aaMax, aaMass[PM_MAX_AA];
  PMSpectrum *nsp1, *nsp2;
  
  if ((sp == NULL) || (sp->peak == NULL)) {
    ARG_ERROR();
    return NULL;
  }

  if (! (nsp1 = PMCopySpectrum(sp))) {  
    MEMORY_ERROR();
    return NULL;
  }
  
  if (! (nsp2 = PMNewShallowSpectrum(sp))) {  
    MEMORY_ERROR();
    return NULL;
  }

  /* put aa masses into array and keep track of max value */
  
  aaMax = 0;
  for (i = naa = 0 ; i < PM_MAX_AA ; i++) {
    float mass = AaDic->table[i].mass;
    if (mass > 0)
      aaMass[naa++] = mass;
    if (mass > aaMax)
      aaMax = mass;
  }
  
  /* mark peaks as invalid   */
  
  for (i = 0 ; i < nsp1->nbPeaks ; i++) {
    nsp1->peak[i].valid = 0;
  }

  /* filter peaks with possible aa doublet        */
  /* note: peaks are ordered by increasing masses */
  
  for (i = 0 ; i < nsp1->nbPeaks ; i++) {
    float mi = nsp1->peak[i].mass;
    for (j = i+1 ; j < nsp1->nbPeaks ; j++) {
      float mj = nsp1->peak[j].mass;
      float dm = (nsp1->dmm >= 0 ? nsp1->dmm * (mj + mi) : -nsp1->dmm);
      if (sFindMass(mj - mi, dm, aaMass, naa)) {
          nsp1->peak[i].valid = 1;
          nsp1->peak[j].valid = 1;
      }
      if ((mj - mi) - dm > aaMax)
        break; /* peak j too far */
    }
  }

  /* retain valid peaks */
  
  for (i = 0 ; i < nsp1->nbPeaks ; i++) {
    if (! nsp1->peak[i].valid)
      continue;
    if (! (nsp2 = PMAddPeakToSpectrum(nsp2, nsp1->peak[i]))) {
      MEMORY_ERROR();
      PMFreeSpectrum(nsp1);
      return NULL;
    }
  }
  
  PMFreeSpectrum(nsp1);
  
  return nsp2;
}

/* -------------------------------------------- */
/* recalibrate spectrum                         */
/* -------------------------------------------- */

PMSpectrum *PMRecalibrateSpectrum(PMSpectrum *sp, float dmm)
{
  int i;
  PMSpectrum *nsp;
  
  if ((sp == NULL) || (sp->peak == NULL)) {
    ARG_ERROR();
    return NULL;
  }

  if (! (nsp = PMCopySpectrum(sp))) {  
    MEMORY_ERROR();
    return NULL;
  }
  
  dmm = 1. + dmm;
  
  for (i = 0 ; i < sp->nbPeaks ; i++) {
    nsp->peak[i].mass *= dmm;
  }
  
  return nsp;
}


/* -------------------------------------------- */
/* Load datas from ".dta" files                 */
/* -------------------------------------------- */

int PMReadDTA1Spectrum (FILE *streamin, PMSpectrum *sp)
{
  return sReadDTASpectrum(streamin, sp);
}

/* -------------------------------------------- */
/* Load datas from ".pkl" files                 */
/* -------------------------------------------- */

int PMReadPKL1Spectrum (FILE *streamin, PMSpectrum *sp)
{
  return sReadPKLSpectrum(streamin, TRUE, sp);
}

int PMReadPKL2Spectrum (FILE *streamin, PMSpectrum *sp)
{
  return sReadPKLSpectrum(streamin, FALSE, sp);
}

/* -------------------------------------------- */
/* Write PMSpectrum to file (Dta format)        */
/* -------------------------------------------- */

int PMWriteDTA1Spectrum (FILE *streamou, PMSpectrum *sp) 
{
  (void) fprintf(streamou,"%.4f %.4f\n", sp->parent.mass, sp->parent.intensity);

  return sWriteDTAPeaks(streamou, sp);
}

/* -------------------------------------------- */
/* Write PMSpectrum to file (Pkl format)        */
/* -------------------------------------------- */

int PMWritePKL1Spectrum (FILE *streamou, PMSpectrum *sp) 
{
  float mass;
  
  mass = (sp->parent.mass + ((float) (sp->charge - 1) * PM_MASS_H)) / (float) sp->charge;
  
  (void) fprintf(streamou,"%.4f %.4f %d\n", mass, sp->parent.intensity, sp->charge);

  return sWriteDTAPeaks(streamou, sp);
}

int PMWritePKL2Spectrum (FILE *streamou, PMSpectrum *sp) 
{
  (void) fprintf(streamou,"%.4f %.4f %d\n", sp->parent.mass, sp->parent.intensity, sp->charge);

  return sWriteDTAPeaks(streamou, sp);
}

/* ==================================================== */
/* Higher level API                                     */
/* ==================================================== */

/* -------------------------------------------- */
/* get a PMSpectrumReader according to format   */
/* -------------------------------------------- */

PMSpectrumReader PMGetSpectrumReader(char *format)
{
   if (! strcmp(format, PM_PKL1_FORMAT))
     return PMReadPKL1Spectrum;

   if (! strcmp(format, PM_PKL2_FORMAT))
     return PMReadPKL2Spectrum;

   if (! strcmp(format, PM_DTA1_FORMAT))
     return PMReadDTA1Spectrum;

   if (! strcmp(format, PM_DTA2_FORMAT))
     return PMReadDTA1Spectrum; // same as PM_DTA1_FORMAT
     
   return NULL;
}

/* -------------------------------------------- */
/* get a PMSpectrumWriter according to format   */
/* -------------------------------------------- */

PMSpectrumWriter PMGetSpectrumWriter(char *format)
{
   if (! strcmp(format, PM_PKL1_FORMAT))
     return PMWritePKL1Spectrum;

   if (! strcmp(format, PM_PKL2_FORMAT))
     return PMWritePKL2Spectrum;

   if (! strcmp(format, PM_DTA1_FORMAT))
     return PMWriteDTA1Spectrum;

   if (! strcmp(format, PM_DTA2_FORMAT))
     return PMWriteDTA1Spectrum; // same as PM_DTA1_FORMAT
     
   return NULL;
}
