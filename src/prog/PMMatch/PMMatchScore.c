/* -----------------------------------------------------------------------
 * $Id: PMMatchScore.c 1773 2013-02-18 00:09:53Z viari $
 * -----------------------------------------------------------------------
 * @file: PMMatchScore.c
 * @desc: PMMatch hit scoring functions 
 *
 * @note: most score functions are still being debugged... not fully tested yet
 * @history:
 * @+ <Gloup> : Aug 06 : first (trivial) version
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PepMap PMMatch hit scoring functions<br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <assert.h>

#include "LX.h"
#include "PM.h"

#include "PMMatch.h"
#include "PMFrag.h"

/* ----------------------------------------------- */
/* clean up peptide sequence					   */
/* ----------------------------------------------- */

static char *sNewCleanSequence(char *seq) {
  char *pepSeq;

  // copy sequence and remove '[' and ']'

  pepSeq = LXStr(seq);
  
  (void) LXEraseChar(pepSeq, '[');
  (void) LXEraseChar(pepSeq, ']');
  
  return pepSeq;
}

/* ----------------------------------------------- */
/* fragment the given sequence and assign peaks    */
/* ----------------------------------------------- */

static LXStackpPtr sFragmentAndAssign(char *sequence, PMSpectrum *spectrum,
                                      PMAminoAlphabet *alpha, Parameter *param)
{
  LXStackpPtr fragments;

  char *pepSeq = sNewCleanSequence(sequence);
  
  if (! (fragments = PMGenerateFragments(pepSeq, alpha))) {
    fprintf(stderr, "# An error occured during fragments generation of sequence %s\n", pepSeq);
    free(pepSeq);
    return NULL;
  }

  LXSortStackp(fragments, PMCompareFragmentMasses);

  (void) PMAssignPeaks(spectrum, fragments, param );
  
  free(pepSeq);

  return fragments;
}

/* ----------------------------------------------- *
 * compute the number of assigned fragments        *
 * ----------------------------------------------- */

// [AV] this function is unused 
// but useful for debug
//
static long sGetNbAssigned( LXStackpPtr fragments )
{
  PMFragmentPtr fragment;

  long nbAssigned = 0;

  LXCurspToBottom( fragments );

  while ( LXReadpUp( fragments, (Ptr *)&fragment ) ) 
    if ( fragment->peak ) nbAssigned++;
  
  return nbAssigned;
}

/* ----------------------------------------------- */
/* score one hit associated to a PST just using    */
/* the PST score                                   */
/* ----------------------------------------------- */

static float sComputePSTHitScore( PMHit *hit ) 
{
  return hit->tag->score;
}

/* ----------------------------------------------- */
/* score one hit associated to a pst using REMT    */
/* Random Match between Experiment and Theoretical */
/* "The Probability for a random Match between an  */
/*  Experimental Theoretical Spectral Pair in      */
/*  Tandem Mass Spectrometry"                      */
/*             Tema Fridman 2005                   */
/* ----------------------------------------------- */

static float sComputeREMTHitScore(PMHit *hit, PMTag *tag, 
                                  LXFastaSequence *fastaSeq,
                                  PMAminoAlphabet *alpha,
                                  PMEnzyme *enzyme,
                                  Parameter *param)
{

  LXStackpPtr fragments;

  if (! (fragments = sFragmentAndAssign(hit->pepMatch, hit->tag->spectrum, alpha, param))) {
    fprintf(stderr, "# cannot fragment sequence %s\n", hit->pepMatch );
    return 0.; // [AV] FIXME : what is the correct result in case of error ?
  }

  // [AV] je pense qu'il manque le calcul !!!
  
  LXFreeStackp(fragments, (LXStackDestroy) PMFreeFragment);
  
  return 0.;
}

/* ----------------------------------------------- *
 * probabilistic model for Dancink 99 scoring algo *
 * (should be in a config file)                    *
 * ----------------------------------------------- */
 
static void sInitDancink99Model(int nbAa, double **p, double *pRandom)
{
  int iFragType, iAa;

  *pRandom = 0.05;
  
  *p = (double *) malloc( sizeof(double) * NB_FRAG_TYPES * nbAa );
  
  for (iAa = 0 ; iAa < nbAa ; iAa++) {
  
    for (iFragType = 0 ; iFragType < NB_FRAG_TYPES ; iFragType++) {
    
      double proba = 0.;
      
      if ((iFragType == B_Fragment) && (iAa != 0))
        proba = 0.6;
      if (iFragType == Y_Fragment)
        proba = 0.8;

      if (proba < *pRandom)
        proba = *pRandom;

      (*p)[iFragType + iAa * NB_FRAG_TYPES] = proba;
      
    }
  }
}

/* ----------------------------------------------- *
 * score one hit associated to a pst using simple  *
 * probabilistic model                             *
 * Random Match between Experiment and Theoretical *
 * "                                               *
 *             Dancink 1999                        *
 * ----------------------------------------------- */


static float sComputeDancink99HitScore(PMHit *hit, PMTag *tag, 
                                       LXFastaSequence *fastaSeq,
                                       PMAminoAlphabet *alpha,
                                       PMEnzyme *enzyme,
                                       Parameter *param)
{
  LXStackpPtr fragments;

  if (! (fragments = sFragmentAndAssign(hit->pepMatch, hit->tag->spectrum, alpha, param))) {
    fprintf(stderr, "# cannot fragment sequence %s\n", hit->pepMatch );
    return -1.; // [AV] FIXME : what is the correct result in case of error ?
  }

  double pRandom, *model;

  sInitDancink99Model( strlen(hit->pepMatch), &model, &pRandom );

  LXCurspToBottom(fragments);

  double logProba = 0.;

  PMFragmentPtr frag;
 
  while (LXReadpUp(fragments, (Ptr *) &frag)) {
  
    double peakProba;
    
    if (frag->peak)
      peakProba = model[frag->type + (frag->index - 1) * NB_FRAG_TYPES] / pRandom;
    else
      peakProba = (1. - model[frag->type + (frag->index - 1) * NB_FRAG_TYPES ]) / (1. - pRandom);

    logProba += log10(peakProba);
  }
  
  (void) LXFreeStackp(fragments, (LXStackDestroy) PMFreeFragment);
  
  free(model);
  
  return logProba;
}


/* ----------------------------------------------- *
 * score one hit associated to a pst using simple  *
 * model based on the sum of the peaks ranks       *
 * ----------------------------------------------- */


static float sRankSumHitScore(PMHit *hit, PMTag *tag, 
                              LXFastaSequence *fastaSeq,
                              PMAminoAlphabet *alpha,
                              PMEnzyme *enzyme,
                              Parameter *param)
{
  LXStackpPtr fragments;

  if (! (fragments = sFragmentAndAssign(hit->pepMatch, hit->tag->spectrum, alpha, param))) {
    fprintf(stderr, "# cannot fragment sequence %s\n", hit->pepMatch );
    return -1; // [AV] FIXME : what is the correct result in case of error ?
  }

  int nbPeaks = hit->tag->spectrum->nbPeaks;

  LXCurspToBottom(fragments);
  
  float rankSum = 0.;

  PMFragmentPtr frag;
 
  while (LXReadpUp(fragments, (Ptr *) &frag)) {
  
    int peakRank;
    
    if (frag->peak)
      peakRank = frag->peak->rank;
    else
      peakRank = nbPeaks;

    rankSum += (float) peakRank;
  }

  long nbFragments = LXStackpSize(fragments);

  float minSum = nbFragments * ( nbFragments + 1 ) / 2. ;
  float maxSum = nbFragments * nbPeaks;

// fprintf( stderr, "@ seq=%s nbF= %ld nbP= %d nbA= %d max= %f min= %f rankSum= %f\n", 
//                 hit->pepMatch, 
//                 nbFragments, nbPeaks, sGetNbAssigned(fragments), maxSum, minSum, rankSum);
  
  (void) LXFreeStackp(fragments, (LXStackDestroy) PMFreeFragment);

  return (maxSum - rankSum) / (maxSum - minSum);
}

/* ----------------------------------------------- *
 * score one hit associated to a pst               *
 * ----------------------------------------------- */

float sComputeHitScore(PMHit *hit, PMTag *tag, 
                       LXFastaSequence *fastaSeq,
                       PMAminoAlphabet *alpha,
                       PMEnzyme *enzyme,
                       Parameter *param)
{

  switch ( param->hitScoreType ) {

    case ( UnknownHitScoreType ) :
    case ( PSTHitScoreType ) :
      return sComputePSTHitScore( hit );

    case ( REMTHitScoreType ) :
      return sComputeREMTHitScore( hit, tag, fastaSeq, alpha, enzyme, param);

    case ( Dancink99HitScoreType ) :
      return sComputeDancink99HitScore( hit, tag, fastaSeq, alpha, enzyme, param );

    case ( RankSumHitScoreType ) :
      return sRankSumHitScore( hit, tag, fastaSeq, alpha, enzyme, param );

  }
  
  return -1.;
}

/* =============================================== */
/* API											   */
/* =============================================== */

/* ----------------------------------------------- */
/* score all hits associated to a pst              */
/* ----------------------------------------------- */

int ScoreHitStack (PMTag *tag, 
                   PMHitStack *hitStack,
                   LXFastaSequence *fastaSeq,
                   PMAminoAlphabet *alpha,
                   PMEnzyme *enzyme,
                   Parameter *param)
{
/*ARGSUSED*/
  long i, n;
  
  n = LXStackpSize(hitStack);
  
  for (i = 0 ; i < n ; i++) {
  
    PMHit *hit = (PMHit *) LXStackpGetValue(hitStack, i);
    
    hit->score = sComputeHitScore(hit, tag, fastaSeq, alpha, enzyme, param);
  } 

  return LX_NO_ERROR;
}

