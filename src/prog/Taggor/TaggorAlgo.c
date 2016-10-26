/* -----------------------------------------------------------------------
 * $Id: TaggorAlgo.c 1366 2011-08-22 16:40:49Z viari $
 * -----------------------------------------------------------------------
 * @file: TaggorAlgo.c
 * @desc: Taggor algorithm program library
 *
 * @history:
 * @+ <romain>  : Feb 03 : first version
 * @+ <Gloup>   : Dec 05 : major revision
 * @+ <Gloup>   : Feb 06 : code cleanup
 * @+ <Gloup>   : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * functions for Taggor algorithm<br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "LX.h"
#include "PM.h"

#include "Taggor.h"

#define NB_SHUFFLE  1000
#define M_AA_MIN    57.0        /* un minorant des masses des aa */

/* ---------------------------------------------------- */
/* Internal structures                                  */
/* ---------------------------------------------------- */

                                             /* ---------------------- */
typedef struct {                             /* Tag Alignment          */
                                             /* ---------------------- */
  int         oligolength;                   /* length of oligo        */
  float       score;                         /* alignment score        */ 
  float       ppm;                           /* mass tolerance         */
  PMAminoAcid oligo[PM_TAG_MAX_SEQ_LEN+2];   /* oligo sequence         */
  PMPeak      *peak[PM_TAG_MAX_SEQ_LEN+2];   /* pointers to peaks      */
  
  float       bonus;						 /* internal use           */
                                             /* ---------------------- */
  											 /* for denovo elongation  */
  LXStackpPtr left;                          /* tags on left (C term)  */
  LXStackpPtr right;                         /* tags on right (N term) */
  
} sTagAlign;

                                             /* ---------------------- */
typedef struct {                             /* DeNovo chain           */
                                             /* ---------------------- */
   int         length;                       /* chain length           */
   float       score;						 /* chain score            */
   int         rank;						 /* chain rank             */
   LXStackpPtr tags;                         /* stack of sTagAlign     */
} sTagChain;


typedef int (*TagCompareFunc)(const void *, const void*);

/* ---------------------------------------------------- */
/* static variable                                      */
/* ---------------------------------------------------- */

static Bool sVerbose = FALSE;


// debug
static Bool sDebug = FALSE;

/* ============================================ */
/* utilities                                    */
/* ============================================ */

/* ============================================ */
/* statistics utilities                         */
/* ============================================ */

/* -------------------------------------------- */

static float sRankStatAlpha(int oligolength, int median)
{
  return (log(2) / (double) (oligolength+1) / (double) median);
}

/* -------------------------------------------- */

static float sRankStatBeta(int oligolength)
{
  return (float) ((oligolength + 1) * (oligolength + 2) / 2);
}

/* -------------------------------------------- */
/* spectrum theoretical statistics              */
/* -------------------------------------------- */

static LXStat sSpectrumStat(PMSpectrum *sp, int oligolength, int threshold) {
  int i;
  double mean;
  LXStat stat;
  
  LXInitStat(&stat);
  
  for (i = 0 ; i < sp->nbPeaks ; i++) {
    if ((threshold <= 0) || (sp->peak[i].rank <= threshold))
      LXUpdateStat(&stat, sp->peak[i].proba);
  }
  
  LXComputeStat(&stat);
  
  // ---------------------------------------------
  // AV : empirical formula based on exponential distribution
  //      assumption (mean(X) = stdev(X) for the spectrum intensity :
  //      mean(X**k) = mean(X)**k
  //      stdev(X**k) = k.mean(X)**k
  //

  if (sVerbose)
    fprintf(stderr, "# spectrum distribution : %ld %g %g\n", stat.npt, stat.mean, stat.stdev);

  mean = 1.;
  for (i = 0 ; i <= oligolength ; i++) {
    mean *= stat.mean;
  }

  stat.mean  = mean;
  stat.stdev = mean * (float) oligolength;
  stat.var   = stat.stdev * stat.stdev;
  
  return stat;
}

/* -------------------------------------------- */
/* spectrum empirical statistics                */
/* -------------------------------------------- */

static LXStat sShufflingStat(PMSpectrum *sp, int oligolength, int threshold) {
  int i, j, n;
  float *x;
  LXStat stat;
  
  if (! (x = NEWN(float, sp->nbPeaks))) {
    MEMORY_ERROR();
    return stat;
  }

  // copy spectrum intensity in array 
  
  for (i = n = 0 ; i < sp->nbPeaks ; i++) {
    if ((threshold <= 0) || (sp->peak[i].rank <= threshold))
      x[n++] = sp->peak[i].proba;
  }
  
  // compute empirical statistics
  
  LXInitStat(&stat);

  LXInitRandom(1); // [AV] to keep always same sequence

  for (i = 0 ; i < NB_SHUFFLE ; i++) {
    float score = 1.;
    for (j = 0 ; j <= oligolength ; j++) {
      int k = LXUniformIRandom(0, n); // uniform random in range [0, n[
      score *= x[k];
    }
    LXUpdateStat(&stat, score);
  }
  
  FREE(x);
  
  LXComputeStat(&stat);

  return stat;
}

/* -------------------------------------------- */
/* update dm/m statistics                       */
/* -------------------------------------------- */

static void sUpdateDmmStat(LXStat *dmmStat, sTagAlign *tagAlign) {
   int i;
   float m0;
   
   m0 = tagAlign->peak[0]->mass;
   
   for (i = 1 ; i <= tagAlign->oligolength ; i++) {
   
     float dm  = (m0 - tagAlign->peak[i]->mass);
     float dmm = (dm/tagAlign->oligo[i-1].mass) - 1.0;
     
     LXUpdateStat(dmmStat, dmm * 1000000.);
     
     m0 = tagAlign->peak[i]->mass;
   }
}

/* ============================================ */
/* tagAlign management                          */
/* ============================================ */

/* -------------------------------------------- */
/* tagAlign score comparison function           */
/* -------------------------------------------- */

static int sTagAlignCompare(const void *x, const void *y)
{
  sTagAlign *tx = (sTagAlign *) x;
  sTagAlign *ty = (sTagAlign *) y;
  
  if (tx->score > ty->score)
    return 1;
  if (tx->score < ty->score)
    return -1;
  return 0;
}

/* same in reverse order */

static int sTagAlignRevCompare(const void *x, const void *y)
{
  return -sTagAlignCompare(x, y);
}

/* -------------------------------------------- */
/* tag free function (for LXHeap compatibility) */
/* -------------------------------------------- */

static void sTagAlignFree(void *p)
{
  sTagAlign *t = (sTagAlign *) p;
  
  if (t) {
    if (t->left)  t->left  = LXFreeStackp(t->left, NULL);
    if (t->right) t->right = LXFreeStackp(t->right, NULL);
    FREE(t);
  }
}

/* -------------------------------------------- */
/* Initialization of new TagAlign               */
/* -------------------------------------------- */

static sTagAlign *sNewTagAlign(PMAminoAcid **oligo, PMPeak **alignment, Parameter *param)
{ 
  int i;

  sTagAlign *tagAlign;

  if (! (tagAlign = NEW(sTagAlign))) {
    MEMORY_ERROR();
    return NULL;
  }

  tagAlign->ppm = param->massTolerance;
  
  tagAlign->left = tagAlign->right = NULL;
  
  if (param->denovo) {     // for denovo we need these stacks
    if (! (   (tagAlign->left  = LXNewStackp(LX_STKP_MIN_SIZE))
           && (tagAlign->right = LXNewStackp(LX_STKP_MIN_SIZE)))) {
      MEMORY_ERROR();
      sTagAlignFree(tagAlign);
      return NULL;
    }
  }

  tagAlign->oligolength = param->oligoLength;

  tagAlign->score = ((param->statMode == RankStat) ? 0. : 1.);
  
  for (i = 0 ; i <= param->oligoLength ; i++) {
    tagAlign->peak[i] = alignment[i];
    if (param->statMode == RankStat)
      tagAlign->score -= alignment[i]->rank;
    else
      tagAlign->score *= alignment[i]->proba;
  }
  
  for (i = 0 ; i < param->oligoLength ; i++) {
    tagAlign->oligo[i] = *oligo[i];
  }

  return tagAlign;
}

/* -------------------------------------------- */
/* compute actual sequence of tagAlign          */
/* note: static buffer -> please copy           */
/* -------------------------------------------- */

static char *sOligoToSeq(sTagAlign *tagAlign)
{
  int i;
  static char sBuffer[BUFSIZ];
 
  for (i = 0 ; i < tagAlign->oligolength ; i++) {
    sBuffer[i] = (char) tagAlign->oligo[i].oneLetterCode;
  }
  
  sBuffer[i]='\000';
  
  return sBuffer;  
}

/* -------------------------------------------- */
/* Make a new PMTag from TagAlign               */
/* -------------------------------------------- */

static PMTag *sNewTag(PMSpectrum *sp, sTagAlign *tagAlign, int id)
{ 
  PMTag *tag;

  if (! (tag = PMNewTag())) {
    MEMORY_ERROR();
    return NULL;
  }

  sprintf(tag->id, "%s.%d", sp->id, id);
  
  strcpy(tag->seq, sOligoToSeq(tagAlign));

  tag->mParent = sp->parent.mass;
  
  tag->mN = sp->parent.mass - tagAlign->peak[0]->mass;
  
  tag->mC = tagAlign->peak[tagAlign->oligolength]->mass - PM_MASS_H3O;
  
  tag->score = tagAlign->score;
  
  tag->spectrum = sp;

  return tag;
}

/* ============================================ */
/* tagAlign compatibility                       */
/* ============================================ */

/* -------------------------------------------- */
/* tells if tag t1 is C-shifted with tag t2     */
/* according to shift                           */
/* -------------------------------------------- */

static int sCShifted(sTagAlign *t1, sTagAlign *t2, int shift)
{
  int i;
  
  for (i = 0 ; i <= (t1->oligolength - shift) ; i++) {
    if (t1->peak[i] != t2->peak[i+shift])
      return 0;
  }
  
  return 1;
}

/* -------------------------------------------- */
/* tells if tag t1 is C-paired with tag t2      */
/* -------------------------------------------- */

static int sCPaired(sTagAlign *t1, sTagAlign *t2)
{
  int shift;
  
  for (shift = 1 ; shift < t1->oligolength ; shift++) {
    if (sCShifted(t1, t2, shift))
      return 1;
  }

  return 0;
}

/* -------------------------------------------- */
/* tells if tag t1 is Linked with tag t2        */
/* -------------------------------------------- */

static int sLinkedTags(sTagAlign *t1, sTagAlign *t2)
{
  return sCShifted(t1, t2, 1) || sCShifted(t2, t1, 1);
}

/* -------------------------------------------- */
/* tells if tag t1 is Ncompatible with tag t2   */
/* note: *this is an approximation*             */
/* -------------------------------------------- */

static int sCCompatible(sTagAlign *t1, sTagAlign *t2)
{
  return (t1->peak[0]->mass + M_AA_MIN <= t2->peak[t2->oligolength]->mass);
}

/* -------------------------------------------- */
/* tells if two tags are compatibles            */
/* -------------------------------------------- */

static int sCompatibleTags(sTagAlign *t1, sTagAlign *t2)
{
  return   sCCompatible(t1, t2) 
        || sCCompatible(t2, t1)
        || sCPaired(t1, t2)
        || sCPaired(t2, t1);
}

/* ============================================ */
/* denovo elongation algorithm                  */
/* ============================================ */

/* -------------------------------------------- */
/* compare chains by score                      */
/* -------------------------------------------- */

static int sCompareChainScore(const void *p1, const void *p2)
{
  sTagChain *c1 = *((sTagChain **) p1);
  sTagChain *c2 = *((sTagChain **) p2);

  if (c1->score == c2->score)
    return 0;
  
  return (c1->score < c2->score ? -1 : 1);
}

/* -------------------------------------------- */
/* compare chains by length                     */
/* -------------------------------------------- */

static int sCompareChainLength(const void *p1, const void *p2)
{
  sTagChain *c1 = *((sTagChain **) p1);
  sTagChain *c2 = *((sTagChain **) p2);

  return (c1->length - c2->length);
}

/* -------------------------------------------- */
/* score of a chain                             */
/* -------------------------------------------- */
static float sScoreChain(sTagChain *chain)
{
  float     sum; 
  sTagAlign *tagAlign;

  sum = 0.;
  
  LXCurspToTop(chain->tags);
    
  while (LXReadpDown(chain->tags, (Ptr *) &tagAlign)) {
    sum += tagAlign->score;
  }

  return sum;
}

/* -------------------------------------------- */
/* length of a chain                            */
/* -------------------------------------------- */
static int sLengthChain(sTagChain *chain)
{
  sTagAlign *tagAlign;

  LXCurspToTop(chain->tags);
  LXReadpDown(chain->tags, (Ptr *) &tagAlign);
  
  return LXStackpSize(chain->tags) + tagAlign->oligolength - 1;
}

/* -------------------------------------------- */
/* normalize chains scores                      */
/* -------------------------------------------- */
static void  sNormalizeChainScore(LXStackpPtr allChains)
{
  float     sum; 
  sTagChain *chain;

  sum = 0.;
  
  LXCurspToTop(allChains);
    
  while (LXReadpDown(allChains, (Ptr *) &chain)) {
    sum += chain->score;
  }
  
  if (sum <= 0.)
    return;

  LXCurspToTop(allChains);
    
  while (LXReadpDown(allChains, (Ptr *) &chain)) {
    chain->score /= sum;
  }
}

/* -------------------------------------------- */
/* compute chains ranks                         */
/* allChains should have been sorted            */
/* -------------------------------------------- */
static void  sComputeChainRanks(LXStackpPtr allChains)
{
  int rank;
  sTagChain *chain;

  rank = 1;
  
  LXCurspToTop(allChains);
    
  while (LXReadpDown(allChains, (Ptr *) &chain)) {
    chain->rank = rank++;
  }
}

/* -------------------------------------------- */
/* compute 										*/
/* lmax = max(chain->length)				    */
/* rmin = rank of chain such that               */
/*        chain->length <=  lmax / 2            */
/* -------------------------------------------- */
static void sChainLmax(LXStackpPtr allChains, int *lmax, int *rmin)
{
  int lmax2;
  
  sTagChain *chain;
  
  // sort chains according to length 
  // and get maximal length
  
  LXSortStackp(allChains, sCompareChainLength);

  LXCurspToTop(allChains);
    
  (void) LXReadpDown(allChains, (Ptr *) &chain);
  
  *lmax = chain->length;
  lmax2 = *lmax / 2;

  // search rmin
  
  *rmin = 1;

  while (LXReadpDown(allChains, (Ptr *) &chain)) {
    if (chain->length <= lmax2)
      break;
    (*rmin)++;
  }  
  
  //  printf("@@@@@ %d %d\n", lmax2, *rmin);

  // restore original order
  
  LXSortStackp(allChains, sCompareChainScore);
}

/* -------------------------------------------- */
/* sTagChain structure management               */
/* -------------------------------------------- */

static sTagChain *sNewChain(LXStackpPtr tags) {

  sTagChain *chain = NEW(sTagChain);
  
  chain->tags = LXCopyStackp(tags, NULL);

  chain->length = sLengthChain(chain);
  
  chain->score = sScoreChain(chain);

  return chain;
}

/* -------------------------------------------- */

static sTagChain *sFreeChain(sTagChain *chain) {

  if (chain->tags)
    chain->tags = LXFreeStackp(chain->tags, NULL);

  FREE(chain);
  
  return NULL;
}

/* -------------------------------------------- */
/* make a denovo chain                          */
/* result is a stack of chains (stacks)         */
/* -------------------------------------------- */

static void sMakeChain(sTagAlign *t, LXStackpPtr tags, LXStackpPtr result)
{

  LXPushpIn(tags, (Ptr) t);  
  
  if (LXIsStackpEmpty(t->left)) {      /* end of recursion   */
    
    LXPushpIn(result, sNewChain(tags));
  }
  
  else {                               /* body of recursion  */
    sTagAlign *next;

    LXCurspToBottom(t->left);
  
    while (LXReadpUp(t->left, (Ptr *) &next))
      sMakeChain(next, tags, result);
 }
  
  LXPoppOut(tags, NULL);
}

/* -------------------------------------------- */
/* denovo algorithm                             */
/* result is a stack of chains (stacks)         */
/* -------------------------------------------- */

static LXStackpPtr sDeNovo(LXHeap *heap)
{
  long i, j;
  LXStackpPtr tags, result;
  
  /* -------------------------- */
  /* build linked lists of tags	*/
  /* -------------------------- */
  
  for (i = 0 ; i < heap->last ; i++) {
  
    sTagAlign *ti = (sTagAlign *) heap->node[i];
    
    for (j = 0 ; j < heap->last ; j++) {
    
      if (i == j)
        continue;

      sTagAlign *tj = (sTagAlign *) heap->node[j];
        
      if (! sCShifted(ti, tj, 1))
        continue;
      
      LXPushpIn(ti->left,  (Ptr) tj);
      LXPushpIn(tj->right, (Ptr) ti);
    }
  }
  
  /* -------------------------- */
  /* make chains                */
  /* -------------------------- */
  
  if (! (    (result = LXNewStackp(LX_STKP_MIN_SIZE))
          && (tags  = LXNewStackp(LX_STKP_MIN_SIZE)))) {
    MEMORY_ERROR();
    return NULL;
  }
  
  for (i = 0 ; i < heap->last ; i++) {
  
    sTagAlign *ti = (sTagAlign *) heap->node[i];
    
    if (LXIsStackpEmpty(ti->right))  // this is a chain starter
      sMakeChain(ti, tags, result);
  }
  
  (void) LXFreeStackp(tags, NULL);

  /* -------------------------- */
  /* normalize chain scores     */
  /* -------------------------- */
  
  sNormalizeChainScore(result);

  /* -------------------------- */
  /* compute ranks              */
  /* -------------------------- */

  LXSortStackp(result, sCompareChainScore);
  
  sComputeChainRanks(result);
  
  return result;
}

/* -------------------------------------------- */
/* -------------------------------------------- */
/* -------------------------------------------- */

static char *sChainToSeq(sTagChain *chain)
{
  int i, k;
  sTagAlign *tagAlign;
  
  static char sBuffer[BUFSIZ];

  LXCurspToTop(chain->tags);
  
  LXReadpDown(chain->tags, (Ptr *) &tagAlign);

  i = tagAlign->oligolength;
  k = tagAlign->oligolength - 1;

  (void) strcpy(sBuffer, sOligoToSeq(tagAlign));
  
  while (LXReadpDown(chain->tags, (Ptr *) &tagAlign)) {
    sBuffer[i++] =  (char) tagAlign->oligo[k].oneLetterCode;   
  }

  sBuffer[i] = '\000';

  return sBuffer;
}


static void sPrintChain(sTagChain *chain)
{
  fprintf(stdout, "@ %f %d %s\n", chain->score, chain->length, sChainToSeq(chain));
}

static void sPrintDeNovo(LXStackpPtr deNovo)
{
  sTagChain *chain;
  
  LXCurspToTop(deNovo);

  fprintf(stderr, "\n");
  
  while (LXReadpDown(deNovo, (Ptr *) &chain))
    sPrintChain(chain);
}

/* ============================================ */
/* spectrum alignment algorithm                 */
/* ============================================ */

// debug
static char *sOligoString(PMAminoAcid **oligo, Parameter *param) {
  static char buffer[256];
  int i;
  for (i = 0 ; i < param->oligoLength ; i++)
    buffer[i] = oligo[i]->oneLetterCode;
  buffer[param->oligoLength] = '\000';
  return buffer;
}

static void sDebugTagAlign(char *head, sTagAlign *tagAlign, Parameter *param) {
  int i;
  char buffer[256];
  for (i = 0 ; i < param->oligoLength ; i++) {
    buffer[i] = tagAlign->oligo[i].oneLetterCode;
  }
  buffer[param->oligoLength] = '\000';
  fprintf(stderr, "%s tag %s : ", head, buffer);
  for (i = 0 ; i <= param->oligoLength ; i++) {
    fprintf(stderr, "%f ", tagAlign->peak[i]->mass);
  }
  fprintf(stderr, "score: %f\n", tagAlign->score);
}

/* -------------------------------------------- */
/* main recursive function to align current     */
/* oligo on spectrum. add found tags in stk     */
/* stack                                        */
/* -------------------------------------------- */

static int sRecAlignOligoY(PMSpectrum *sp, PMAminoAcid **oligo, PMPeak **alignment, 
                           int peakindex, int oligoindex, Parameter *param,
                           LXHeap *heap, LXStat *stat)
{
  int i;
  float maa, m0;

  /* --------------------------------------------- */
  /* end of recursion : we push a new tag in heap  */
  /* --------------------------------------------- */

  if (oligoindex >= param->oligoLength) {

    sTagAlign *tagAlign;
    
    if (! (tagAlign = sNewTagAlign(oligo, alignment, param)))
      return LX_MEM_ERROR;
      
    LXUpdateStat(stat, tagAlign->score); // keep stat on found tags
    LXAddInHeap(heap, tagAlign);

if (sDebug) {
//  sDebugTagAlign("found", tagAlign, param);
}
    
    return LX_NO_ERROR;
  }

  /* ------------------ */
  /* body of recursion  */
  /* ------------------ */

  maa = oligo[oligoindex]->mass;
  m0 = alignment[oligoindex]->mass;

  for (i = peakindex ; i >= 0 ; i--) { // TODO peakindex - 1

    /* property : the peaks are ordered by increasing masses */
    /*            therefore m1 <= m0                         */

    float m1 = sp->peak[i].mass;
        
    /* lets note Dmin(i) the minimum allowed mass difference                */
    /*           Dmin(i) = {(m0 - m1) - (m0 + m1).dmm}                      */
    /* property: if maa < Dmin(i) then maa < Dmin(j) for j <= i             */
    /*           (because Dmin is an decreasing function of m1)             */
    /*           therefore it is useless to test further peaks              */

    float dmini;

    if (sp->dmm >= 0)
      dmini = (m0 - m1) - (m0 + m1) * sp->dmm;
    else
      dmini = (m0 - m1) + sp->dmm;
    
    if (maa < dmini)
      return LX_NO_ERROR;

    /* lets note Dmax(i) the maximum allowed mass difference                */
    /*           Dmax(i) = {(m0 - m1) + (m0 + m1).dmm}                      */
    /* then if maa > Dmax(i) the mass difference is too small but further   */
    /* peaks can achieve the result. We should continue                     */

    float dmaxi;
    
    if (sp->dmm >= 0)
      dmaxi = (m0 - m1) + (m0 + m1) * sp->dmm;
    else
      dmaxi = (m0 - m1) - sp->dmm;
    
    if (maa > dmaxi)
      continue; 

    /* D(i) is in the right range                                           */
    /* we should record this peak and recurse to next symbol                */
    
    alignment[oligoindex+1] = sp->peak + i;

    sRecAlignOligoY(sp, oligo, alignment, i-1, oligoindex+1, param, heap, stat);
  }
  
  return LX_NO_ERROR;
}


/* -------------------------------------------- */
/* align current oligo on spectrum and add      */
/* results in heap                              */
/* -------------------------------------------- */

static int sAlignOligo(PMSpectrum *sp, PMAminoAcid **oligo, Parameter *param, LXHeap *heap, LXStat *stat) {

  int peakindex;
  PMPeak *alignment[PM_TAG_MAX_SEQ_LEN+2];

static char *sbuf;
sbuf = sOligoString(oligo, param);

//sDebug = (! strcmp(sbuf, "AGA")) || (! strcmp(sbuf, "SAI"));

if (sDebug)
  fprintf(stderr, "debug %s\n", sbuf);

  for (peakindex = sp->nbPeaks - 1 ; peakindex >= param->oligoLength ; peakindex--) {
    
    alignment[0] = sp->peak + peakindex;

if (sDebug)
  fprintf(stderr, "start at %f\n", sp->peak[peakindex].mass);


    sRecAlignOligoY(sp, oligo, alignment, peakindex - 1, 0, param, heap, stat);
  }
  
  return LX_NO_ERROR;
}

/* ============================================ */
/* tag generation algorithm                     */
/* ============================================ */

/* -------------------------------------------- */
/* prepare spectrum for algo  :                 */
/* - remove ions with mass > parent             */
/* - add two dummy peaks at mass H and parent   */
/* -------------------------------------------- */

static PMSpectrum *sPrepareForAlgo(PMSpectrum *sp, Parameter *param)
{
  int i;
  float upperMass;
  PMPeak peak;
  PMSpectrum  *nsp;

  peak.valid = 1;
  peak.intensity = 0.0;
  peak.proba = 1. / (1. + sp->nbPeaks);
  peak.rank = param->statRParam;

  if (sp->dmm >= 0)
    upperMass = sp->parent.mass * (1. - sp->dmm);
  else
    upperMass = sp->parent.mass + sp->dmm;
  
  nsp = PMNewShallowSpectrum(sp);

  peak.mass = PM_MASS_H3O;

  nsp = PMAddPeakToSpectrum(nsp, peak);
  
  for (i = 0 ; i < sp->nbPeaks ; i++) {
    if (sp->peak[i].mass >= upperMass)
      continue;
    if (! (nsp = PMAddPeakToSpectrum(nsp, sp->peak[i]))) {
      MEMORY_ERROR();
      return PMFreeSpectrum(nsp);
    }
  }
  
  peak.mass = sp->parent.mass;
  
  nsp = PMAddPeakToSpectrum(nsp, peak);
  
  return nsp;
}

/* -------------------------------------------- */
/* main recursive function to generate oligos   */
/* and align them on spectrum                   */
/* -------------------------------------------- */

static int sGenerateTestOligos(PMSpectrum *sp, PMAminoAlphabet *alpha, PMAminoAcid **oligo,
                               int oligoindex, Parameter *param, LXHeap *heap, LXStat *stat)
{
  char *symb;
  char alphabet[PM_MAX_AA+1];

  /* ------------------------------------------ */
  /* end of recursion : align oligo on spectrum */
  
  if (oligoindex >= param->oligoLength) {
    return sAlignOligo(sp, oligo, param, heap, stat);
  }

  /* ------------------------------------------ */
  /* body of recursion : generate oligo         */

  /* [AV] change this to 'PMGetFullAlphabet'    */
  /* to take modified AA into account           */
  /* but don't forget to check the modification */
  /* location within the recursion loop below   */

  (void) strcpy(alphabet, PMGetNormalAlphabet(alpha));
  
  for (symb = alphabet ; *symb ; symb++) {

    if ((*symb == 'L') || (*symb == 'Q'))
      continue;

    oligo[oligoindex] = alpha->table + (int) *symb;
      
    int res = sGenerateTestOligos(sp, alpha, oligo, oligoindex+1, param, heap, stat);
      
    if (res != LX_NO_ERROR)
      return res;
  }
  
  return LX_NO_ERROR;
}

/* -------------------------------------------- */
/* perform statistics on tags score             */
/* -------------------------------------------- */

static void sTagStatistics(PMSpectrum *sp, LXHeap *heap, Parameter *param, LXStat empStat)
{
  long i;
  LXStat shufStat, theoStat, chooseStat;

  // empirical raw tags statistics

  if (sVerbose)
    fprintf(stderr, "# empirical tags stats : %ld %g %g\n", empStat.npt , empStat.mean, empStat.stdev);

  // empirical spectrum statistics
  
  shufStat = sShufflingStat(sp, param->oligoLength, param->statESTParam);

  if (sVerbose)
    fprintf(stderr, "# empirical spectrum stats : %ld %g %g\n", shufStat.npt , shufStat.mean, shufStat.stdev);
  
  // theoretical spectrum statistics

  theoStat = sSpectrumStat(sp, param->oligoLength, param->statESTParam);

  if (sVerbose)
    fprintf(stderr, "# theo spectrum stats : %ld %g %g\n", theoStat.npt , theoStat.mean, theoStat.stdev);
  
  // rank statistics
  
  float alpha = sRankStatAlpha(param->oligoLength, param->statRParam);
  float beta  = sRankStatBeta(param->oligoLength);
  
  if (sVerbose)
    fprintf(stderr, "# rank stats : %d %g %g\n", param->statRParam, alpha, beta);
  
  //
  // compute tag score
  //

  if (param->statMode == RankStat) {
    //
    // rank model statistics
    //  
    int ranks[PM_TAG_MAX_SEQ_LEN];
    for (i = 0 ; i < heap->last ; i++) {
      sTagAlign *tagAlign = (sTagAlign *) heap->node[i];
      tagAlign->score = (float) exp(alpha * (tagAlign->score + beta));
//sDebugTagAlign("score", tagAlign, param);
    }
  }
  else {
    //
    // compute z-score with one of these statistics
    //
    if (param->statMode == EmpiricalStat) 
      chooseStat = (empStat.npt > 10) ? empStat : theoStat;
    else if (param->statMode == SpectrumStat)
      chooseStat = shufStat;
    else
      chooseStat = theoStat;
   
    for (i = 0 ; i < heap->last ; i++) {
      sTagAlign *tagAlign = (sTagAlign *) heap->node[i];
      tagAlign->score = (tagAlign->score - chooseStat.mean) / chooseStat.stdev;
    }
  }
}

/* -------------------------------------------- */
/* compute tag correlation and modify scores    */
/* -------------------------------------------- */

static void sTagCorrelations(LXHeap *heap)
{
  long i, j;

  // compute bonus for each tag
  
  for (i = 0 ; i < heap->last ; i++) {
  
    sTagAlign *ti = (sTagAlign *) heap->node[i];
    ti->bonus = 0.;
    
    for (j = 0 ; j < heap->last ; j++) {
    
      if (i == j)
        continue;
      
      sTagAlign *tj = (sTagAlign *) heap->node[j];
      
      if (sLinkedTags(ti, tj))
        ti->bonus += (MAX(0, tj->score / 2.));
    }
  }
  
  // update scores
  
  for (i = 0 ; i < heap->last ; i++) {
    sTagAlign *ti = (sTagAlign *) heap->node[i];
    ti->score += ti->bonus;
  }

  // rebuild heap
  
  LXRebuiltHeap(heap, sTagAlignCompare);
}

/* -------------------------------------------- */
/* modify tag score according to deNovo chains  */
/* -------------------------------------------- */
static void sTagDeNovo(LXHeap *heap, LXStackpPtr deNovo, Parameter *param)
{
  long i;
  sTagChain *chain;

  for (i = 0 ; i < heap->last ; i++) {
    sTagAlign *ti = (sTagAlign *) heap->node[i];
    ti->bonus = 0;
  }

  // get min rank foreach tag
  // note: chains come with lowest rank first
  
  LXCurspToTop(deNovo);
    
  while (LXReadpDown(deNovo, (Ptr *) &chain)) {

    sTagAlign *tagAlign;
    
    LXCurspToTop(chain->tags);

    while (LXReadpDown(chain->tags, (Ptr *) &tagAlign)) {
      if (tagAlign->bonus == 0)
        tagAlign->bonus = chain->rank;
    }
  }

  // compute the alpha coefficient
  //
  
  // int lmax;
  // sChainLmax(deNovo, &lmax, &rmin); // AV: Not yet
  
  // transform bonus into scores
  
  for (i = 0 ; i < heap->last ; i++) {
    sTagAlign *ti = (sTagAlign *) heap->node[i];
    if (ti->bonus == 0)
      ti->score = 0.; // normally impossible
    else
      ti->score *= (float) exp ((1.0 - ti->bonus) / param->denovoRmin);
  }

  // rebuild heap
  
  LXRebuiltHeap(heap, sTagAlignCompare);
}



/* -------------------------------------------- */
/* Tag lexicon comparison function              */
/* -------------------------------------------- */
static int sCompareLexicons(const void *p1, const void *p2)
{
  PMTag *t1 = (PMTag *) p1;
  PMTag *t2 = (PMTag *) p2;
  
  return strcmp(t1->seq, t2->seq);
}

/* -------------------------------------------- */
/* compare masses with tolerance                */
/* [AV] this function may go into library...    */
/* -------------------------------------------- */
static Bool sEqualMasses(float m1, float m2, float offset, float tolerance)
{
  float dm, dmm;
  
  if (tolerance >= 0)
    dmm = ((m1 + m2) / 2. + offset + PM_DMM_OFFSET) * tolerance;
  else
    dmm = -tolerance;
    
  dm = ABS(m1 - m2);
  
  return (dm <= dmm);
}

/* -------------------------------------------- */
/* Tag full comparison function                 */
/* -------------------------------------------- */
static int sCompareTags(const void *p1, const void *p2)
{
  PMTag *t1 = (PMTag *) p1;
  PMTag *t2 = (PMTag *) p2;
  
  return ((    (strcmp(t1->seq, t2->seq) == 0)
            && sEqualMasses(t1->mN, t2->mN, t1->mParent, t1->spectrum->dmm)
            && sEqualMasses(t1->mN, t2->mN, 0., t1->spectrum->dmm))
         ? 0 : 1);
}

/* -------------------------------------------- */
/* check if tag is already in stack             */
/* -------------------------------------------- */
static Bool sIsDuplicateTag(PMTag *tag, PMTagStack *stk, Parameter *param)
{
  if (param->uniqueMode == NoUnique)
    return FALSE;

  TagCompareFunc compar;
  
  if (param->uniqueMode == UniqueSeq)
    compar = sCompareLexicons;
  else
    compar = sCompareTags;

  LXCurspToTop(stk);
  
  return LXSearchDownStackp(stk, tag, compar);
}


/* ============================================ */
/* public API                                   */
/* ============================================ */

/* -------------------------------------------- */
/* Compute tags (Y) for a given spectrum        */
/* -------------------------------------------- */

PMTagStack* ComputeTags(PMSpectrum *sp, PMAminoAlphabet *alpha, Parameter *param, LXStat *dmmStat)
{
  long        itag, heapSize;
  LXHeap      *heap;
  PMSpectrum  *nsp;
  PMTagStack  *stk;
  PMAminoAcid *oligo[PM_TAG_MAX_SEQ_LEN+2];
  sTagAlign   *tagAlign;
  LXStat      empStat;

  sVerbose = param->verbose;
  
  /* check arguments */
  
  if (! (sp && sp->peak && alpha)) {
    ARG_ERROR();
    return NULL;
  }

  if ((param->oligoLength <= 0) || (param->nbResults <= 0)) {
    ARG_ERROR();
    return NULL;
  }

  /* allocate heap for results */
  
  heapSize = MAX(2 * param->nbResults, 100); // [AV]: should be another param
  
  if (! (heap = LXNewHeap(heapSize, sTagAlignCompare, sTagAlignFree))) {
    MEMORY_ERROR();
    return NULL;
  }
  
  /* add dummy peaks for algo */
  
  nsp = sPrepareForAlgo(sp, param);
  
  /* everything ready, now go ahead */

  if (param->verbose) 
    fprintf(stderr, "# Computing tags (%s=%.1f nbPeaks=%d) ", 
      (nsp->dmm >= 0 ? "ppm" : "amu"),
      (nsp->dmm >= 0 ? nsp->dmm * 1e6 : -nsp->dmm),
      nsp->nbPeaks);

  LXInitStat(&empStat);

  if (sGenerateTestOligos(nsp, alpha, oligo, 0, param, heap, &empStat) != LX_NO_ERROR) {
    (void) LXFreeHeap(heap);
    return NULL;
  }

  LXComputeStat(&empStat);

  if (param->verbose)
    fprintf(stderr, "done\n");

  /* compute statistics */
  
  sTagStatistics(nsp, heap, param, empStat);

  /* compute correlations */

  if (param->tagCorrelation)
    sTagCorrelations(heap);

  /* compute denovo     */
    
  if (param->denovo) {
    LXStackpPtr deNovo = sDeNovo(heap);
  
    // sPrintDeNovo(deNovo);
  
    sTagDeNovo(heap, deNovo, param);

    (void) LXFreeStackp(deNovo, (LXStackDestroy) sFreeChain);
  }

  /* reformat TagAlign to Tag and keep in a stack */

  if ( ! (stk = PMNewTagStack())) {
    MEMORY_ERROR();
    (void) LXFreeHeap(heap);
    (void) PMFreeSpectrum(nsp);
    return NULL;
  }
  
    /* rebuild heap in reverse order */
  
  LXRebuiltHeap(heap, sTagAlignRevCompare);
  
  itag = 1;
  
  while (tagAlign = LXPopHeapRoot(heap)) {

    if (param->verbose)
      sDebugTagAlign("end", tagAlign, param);

    // tagAlign come by decreasing score

    if (    (itag > param->nbResults)
        ||  (tagAlign->score < param->minScore))
      break;
      
    PMTag *tag = sNewTag(nsp, tagAlign, itag);
      
    if (sIsDuplicateTag(tag, stk, param)) {
      (void) PMFreeTag(tag); // discard this duplicate
    }
    else {
      LXPushpIn(stk, tag);

      itag++;

      if (dmmStat)
        sUpdateDmmStat(dmmStat, tagAlign);
    }
    
    sTagAlignFree(tagAlign);
    
  }
  
  (void) LXFreeHeap(heap);

  (void) PMFreeSpectrum(nsp);
  
  return stk;
}

/* -------------------------------------------- */
/* Spectrum  preprocessing                      */
/* -------------------------------------------- */

PMSpectrum *PreprocessSpectrum(PMSpectrum *sp, PMAminoAlphabet *alpha, Parameter *param)
{
  int nenh;
  PMSpectrum *nsp0, *nsp1, *nsp2, *nsp3;
  
  sVerbose = param->verbose;

  if (! (sp && sp->peak)) {
    ARG_ERROR();
    return NULL;
  }

  /* ------------------------------ */
  /* - Step 0 -                     */
  /* filter spectrum on low mass    */
  /* sp -> nsp0                     */
  
  nsp0 = PMFilterMassSpectrum(sp, param->massThreshold,
                                  param->massThreshold + 2 * sp->parent.mass);
  if (! nsp0) {
    MEMORY_ERROR();
    return NULL;
  }

  /* ------------------------------ */
  /* - Step 1 -                     */
  /* filter spectrum on relative    */
  /* intensity                      */
  /* nsp0 -> nsp1                   */
  
  nsp1 = PMFilterProbaSpectrum(nsp0, param->peakThreshold, 1.);
  
  if (! nsp1) {
    MEMORY_ERROR();
    return NULL;
  }

  /* ---------------------------------------------- */
  /* - Step 3 -                                     */
  /* filter on ranks                                */
  /* nsp1 -> nsp2                                   */
  
  if (param->rankThreshold > 0) {
    nsp2 = PMFilterRankSpectrum(nsp1, 0, param->rankThreshold);
  }
  else {
    nsp2 = PMCopySpectrum(nsp1);
  }

  if (! nsp2) {
    PMFreeSpectrum(nsp0);
    PMFreeSpectrum(nsp1);
    MEMORY_ERROR();
    return NULL;
  }

  /* ------------------------------ */
  /* normalize                      */
  /* and update ranks               */

  (void) PMProbabilizeSpectrum(nsp2);
  (void) PMRankizeSpectrum(nsp2);

  /* ---------------------------------------------- */
  /* - Step 2 -                                     */
  /* filter on aa doublets                          */
  /* keep only peaks that have a mass difference    */
  /* of 1 aa with at least another peak             */
  /*                                                */
  /* this will speedup the search without changing  */
  /* the results.                                   */
  /*                                                */
  /* nsp2 -> nsp3                                   */

  nsp3 = PMFilterDoubletSpectrum(nsp2, alpha);

  if (! nsp3) {
    PMFreeSpectrum(nsp0);
    PMFreeSpectrum(nsp1);
    PMFreeSpectrum(nsp2);
    MEMORY_ERROR();
    return NULL;
  }

  /* [AV] no renormalization here ! */  

  /* ---------------------------------------------- */
  /* - Step 4 -                                     */
  /* enhance intensity of YB peaks                  */

#if 0

  !! ----------------------- !!
  !! DOES NOT WORK YET       !!
  !! ----------------------- !!
  
  for (i = nenh = 0 ; i < nsp2->nbPeaks ; i++) {
    float mi = nsp2->peak[i].mass;
    for (j = 0 ; j < nsp1->nbPeaks ; j++) { // AV : should scan nsp1
      float mj = nsp1->peak[j].mass;
      float ddm = nsp2->dmm * (mj + mi + nsp2->parent.mass); // AV : dmm >< 0
      float dm = (mi + mj - nsp2->parent.mass - PM_MASS_H);
      if (dm < 0) dm = -dm;
      if (dm <= ddm) {
        nsp3->peak[i].proba *= 10.;
        nenh++;
        break;
      }
    }
  }

#else

  nenh = 0;

#endif

  if (param->verbose)
    fprintf(stderr, "# filtering : %d %d %d %d %d %d\n", sp->nbPeaks,
                 nsp0->nbPeaks, nsp1->nbPeaks, nsp2->nbPeaks, nsp3->nbPeaks, nenh);

  PMFreeSpectrum(nsp0);
  PMFreeSpectrum(nsp1);
  PMFreeSpectrum(nsp2);

  return nsp3; 
}

