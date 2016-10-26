/* -----------------------------------------------------------------------
 * $Id: PMClustMain.c 1810 2013-02-25 10:04:43Z viari $
 * -----------------------------------------------------------------------
 * @file: PMClust.c
 * @desc: PepMap cluster main program
 *
 * @history:
 * @+ <Wanou> : Jan 02 : first version
 * @+ <Gloup> : Aug 06 : complete code revision
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PepMap cluster program (PMClust)<br>
 *
 * @docend:
 */


#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 

#include "LX.h"
#include "PM.h"

#include "PMClust.h"

/* -------------------------------------------------------------------- */
/* Local functs                                                         */
/* -------------------------------------------------------------------- */

/* ----------------------------------------------- */
/* comparison function for sorting ExtendedHit     */
/* ** taking sequence frame into account **        */
/* ----------------------------------------------- */

static int sSortXhitsWithFrame(const void *h1, const void *h2)
{
  ExtendedHit **x1 = (ExtendedHit **) h1;
  ExtendedHit **x2 = (ExtendedHit **) h2;
  PMSeqId *s1 = (*x1)->seqId;
  PMSeqId *s2 = (*x2)->seqId;
  
  int cmp;
  
  // first sort by seqId->name

  if ((cmp = strcmp(s1->name, s2->name)) != 0)
    return cmp;
    
  // sort by strand
  
  if ((cmp = (s1->strand - s2->strand)) != 0)
    return cmp;

  // sort by frame

  if ((cmp = (s1->frame - s2->frame)) != 0)
    return cmp;
  
  // finally sort by from position
  
  if ((*x1)->from == (*x2)->from)
    return 0;
    
  return (((*x1)->from < (*x2)->from) ? -1 : 1);
}

/* ----------------------------------------------- */
/* comparison function for sorting ExtendedHit     */
/* ** ignoring sequence frame **                   */
/* ----------------------------------------------- */

static int sSortXhitsWithoutFrame(const void *h1, const void *h2)
{
  ExtendedHit **x1 = (ExtendedHit **) h1;
  ExtendedHit **x2 = (ExtendedHit **) h2;
  PMSeqId *s1 = (*x1)->seqId;
  PMSeqId *s2 = (*x2)->seqId;
  
  int f1 = s1->frame;
  int f2 = s2->frame;
  
  s1->frame = s2->frame = 0;

  int cmp =  sSortXhitsWithFrame(h1, h2);
  
  s1->frame = f1;
  s2->frame = f2;
  
  return cmp;
}

/* ----------------------------------------------- */
/* clean cluster                                   */
/* because some upstream partial hits maybe wrong  */
/* ----------------------------------------------- */

static void sCleanCluster(Cluster *cluster, Parameter *param)
{
  ExtendedHit *xhit, *full;

  // search first full hit
  
  full = NULL;

  LXCurspToTop(cluster->xhits);

  while (LXReadpDown(cluster->xhits, (Ptr *) &xhit)) {
    if (xhit->hit->type == FullHit) {
      full = xhit;
      break;
    }
  }

  // no full hit -> empty stack
  
  if (full == NULL) {
    cluster->xhits->top = 0;
    return;
  }

  // skip over partial hits that are too far
  
  LXCurspToTop(cluster->xhits);

  while (LXReadpDown(cluster->xhits, (Ptr *) &xhit)) {
    long delta = full->from - xhit->to;
    if (delta <= param->distance)
      break;
  }

  cluster->xhits->top = cluster->xhits->cursor + 1;
}

/* ----------------------------------------------- */
/* count full hits in cluster                      */
/* ----------------------------------------------- */

static int sCountFullHits(Cluster *cluster)
{
  int count;
  ExtendedHit *xhit;
  
  count = 0;
  LXCurspToTop(cluster->xhits);
  
  while (LXReadpDown(cluster->xhits, (Ptr *) &xhit)) {
    if (xhit->hit->type == FullHit)
      count++;
  }

  return count;
}

/* ----------------------------------------------- */
/* count pephits in cluster                        */
/* ----------------------------------------------- */

static int sCountPepHits(Cluster *cluster)
{
  long from, to;
  int  count;
  ExtendedHit *xhit;
  
  from = to = -1;
  count = 0;
  LXCurspToTop(cluster->xhits);
  
  while (LXReadpDown(cluster->xhits, (Ptr *) &xhit)) {
    if (xhit->hit->type == FullHit) {
      if (   (from < 0) 
          || (xhit->from != from)
          || (xhit->to   != to)) {
        count++;
        from = xhit->from;
        to   = xhit->to;
      }
    }
  }

  return count;
}

/* ----------------------------------------------- */
/* sum hit scores in cluster                       */
/* ----------------------------------------------- */

static float sSumScore(Cluster *cluster)
{
  float score;
  ExtendedHit *xhit;
  
  score = 0.;
  LXCurspToTop(cluster->xhits);
  
  while (LXReadpDown(cluster->xhits, (Ptr *) &xhit)) {
    if (xhit->hit->type == FullHit) {
      score += xhit->hit->score;
    }
  }

  return score;
}

/* ----------------------------------------------- */
/* score cluster                                   */
/* ----------------------------------------------- */

static void sScoreCluster(Cluster *cluster, Parameter *param)
{
  cluster->nbHits    = cluster->xhits->top;
  cluster->nbFull    = sCountFullHits(cluster);
  cluster->nbPephits = sCountPepHits(cluster);
  cluster->score     = sSumScore(cluster);
}

/* ----------------------------------------------- */
/* print hit with Nucleotidic positions            */
/* ----------------------------------------------- */

static void sPrintNucleotidicHit(ExtendedHit *xhit)
{
  long oldMn, oldSn, oldMc, oldSc;
   
  oldMn = xhit->hit->posMn;
  oldSn = xhit->hit->posSn;
  oldMc = xhit->hit->posMc;
  oldSc = xhit->hit->posSc;

  UtilNucleotidicPosition(xhit->seqId,   xhit->hit->posMn,    xhit->hit->posMc, 
                                       &(xhit->hit->posMn), &(xhit->hit->posMc));
  UtilNucleotidicPosition(xhit->seqId,   xhit->hit->posSn,    xhit->hit->posSc, 
                                       &(xhit->hit->posSn), &(xhit->hit->posSc));

  (void) PMWriteHit(stdout, xhit->hit);
  
  xhit->hit->posMn = oldMn;
  xhit->hit->posSn = oldSn;
  xhit->hit->posMc = oldMc;
  xhit->hit->posSc = oldSc;
}

/* ----------------------------------------------- */
/* print cluster                                   */
/* ----------------------------------------------- */

static int sPrintCluster(Cluster *cluster, Parameter *param, int clustIndex)
{
  ExtendedHit *xhit;

  if (cluster->xhits->top <= 0)
    return clustIndex;
    
  if (cluster->nbFull < param->minFull)
    return clustIndex;

  if (cluster->nbPephits < param->minPephits)
    return clustIndex;

  clustIndex++;
  
  printf("CLUSTER %d %d %d %d %g\n", clustIndex,
                                     cluster->nbHits, cluster->nbFull, 
                                     cluster->nbPephits, cluster->score);

  LXCurspToTop(cluster->xhits);

  while (LXReadpDown(cluster->xhits, (Ptr *) &xhit)) {
    (void) PMWriteSeqId(stdout, xhit->seqId);
    (void) PMWriteTag(stdout, xhit->tag);
    if (param->isProteic) {
      (void) PMWriteHit(stdout, xhit->hit);
    }
    else {
      sPrintNucleotidicHit(xhit);
    }
  }
  
  printf("%cEndOfCluster\n", PM_END_OF_RECORD);
  
  return clustIndex;
}

/* ----------------------------------------------- */
/* cleanup and score and print cluster             */
/* ----------------------------------------------- */
static int sProcessCluster(Cluster *cluster, Parameter *param, int clustIndex)
{
  LXReverseStackp(cluster->xhits);
  sCleanCluster(cluster, param);
  sScoreCluster(cluster, param);
  return sPrintCluster(cluster, param, clustIndex);
}

/* ----------------------------------------------- */
/* clusterize                                      */
/* ----------------------------------------------- */

static int sMakeClusters(LXStackp *hits, Parameter *param)
{
  int nbClust = 0;
  
  ExtendedHit *xhit, *currentFull;
  Cluster     cluster;

  cluster.xhits = LXNewStackp(LX_STKP_MIN_SIZE);
  
  currentFull = NULL;
  
  LXCurspToTop(hits);
  
  while (LXReadpDown(hits, (Ptr *) &xhit)) {
  
    Bool printIt;
    
    if (xhit->hit->type == InvalidHit) {         /* ignore these guys  */
      continue;
    }
  
    if (currentFull == NULL) {                   /* first full hit     */
      printIt = FALSE;
    }
												/* change sequence     */
    else if (strcmp(currentFull->seqId->name, xhit->seqId->name)) {
      printIt = TRUE;
    }
												/* change strand       */
    else if (    (! param->isProteic)
              && (currentFull->seqId->strand != xhit->seqId->strand)) {
      printIt = TRUE;
    }

												/* change frame       */
    else if (    (! param->isProteic)
              && (param->framed)
              && (currentFull->seqId->frame != xhit->seqId->frame)) {
      printIt = TRUE;
    }

    else {										/* check distance     */
      long delta = xhit->from - currentFull->to;
      printIt = (delta > param->distance);
    }

    
    if (printIt) {
      nbClust = sProcessCluster(&cluster, param, nbClust);
      
      cluster.xhits->top = 0;  /* quick way to reset stack  */
      currentFull = NULL;      /* reset cluster lower bound */
    }

    if (    (xhit->hit->type == FullHit)
         || (param->addPartial)) {
      (void) LXPushpIn(cluster.xhits, (Ptr) xhit);
    }
    
    currentFull = (xhit->hit->type == FullHit) ? xhit : currentFull;

  }

  nbClust = sProcessCluster(&cluster, param, nbClust);  // don't forget last cluster

  (void) LXFreeStackp(cluster.xhits, NULL);
  
  return nbClust;
}


/* -------------------------------------------------------------------- */
/* Main Entry point                                                     */
/* -------------------------------------------------------------------- */

main(int argc, char *argv[])
{  
  extern char *optarg;
  
  int opt, nbClust;

  LXDict *tagDictionary, *seqDictionary;
  
  Parameter param;
  
  char inputFilename[BUFSIZ],
       outputFilename[BUFSIZ];

  /* --------------------------------- */
  /* setup default parameters          */
  
  param.distance           = DFT_DISTANCE;
  param.minFull            = DFT_MIN_FULL;
  param.minPephits         = DFT_MIN_PEPHITS;
  param.isProteic          = FALSE;
  param.framed             = FALSE;
  param.addPartial         = FALSE;
  param.verbose            = FALSE;
  
  *inputFilename  = '\000';
  *outputFilename = '\000';

  /* --------------------------------- */
  /* get user's parameters             */

  while ((opt = getopt(argc, argv, "d:fHhi:o:pPt:T:v")) != -1) {
  
    switch (opt) {
      
      case 'd':                                   // clustering distance
        if (   (sscanf(optarg, "%d", &param.distance) != 1)
            || (param.distance <= 0)) {
          fprintf(stderr, "# Wrong value for clustering distance\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'f' :                                  // force frame clustering
        param.framed = TRUE;
        break;

      case 'H' :                                  // help
      case 'h' :
        PrintHelp();
        exit(0);
        break;
  
      case 'i' :                                  // input (hits) filename
        (void) strcpy(inputFilename, optarg);
        if(! LXAssignToStdin(inputFilename)) {
          fprintf(stderr,"# Cannot open : %s\n", inputFilename);
          exit (LX_IO_ERROR); 
        }
        break;
        break;

      case 'o' :                                  // output (hits) filename
	    (void) strcpy(outputFilename, optarg);
        if (! LXAssignToStdout(outputFilename)) {
          fprintf(stderr,"# Cannot create : %s\n", outputFilename);
          exit (LX_IO_ERROR);
        }
        break;

      case 'p' :                                  // add partial
        param.addPartial = TRUE;
        break;

      case 'P' :                                  // Protein mode
        param.isProteic = TRUE;
        break;

      case 't':                                   // min full hits
        if (   (sscanf(optarg, "%d", &param.minFull) != 1)
            || (param.minFull <= 0)) {
          fprintf(stderr, "# Wrong value for minimum full hits\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'T':                                   // min pephits
        if (   (sscanf(optarg, "%d", &param.minPephits) != 1)
            || (param.minPephits <= 0)) {
          fprintf(stderr, "# Wrong value for minimum pephits\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'v' :                                  // verbose mode
        param.verbose = TRUE;
        break;

      case '?' :                                  // oops !
        ExitUsage(LX_ARG_ERROR);
        break;      
    }
  }

  /* --------------------------------- */
  /* adjust parameters                 */

  /* for proteic cluster :             */
  /* set distance to infinity          */

  if (param.isProteic) {
    param.distance = BIG_INT32;
  }

  /* --------------------------------- */
  /* go ahead                          */

  StartupBanner(argc, argv);
  
  tagDictionary = LXNewDict(UtilCompareTag);
  seqDictionary = LXNewDict(UtilCompareSeqId);
  
  /* --------------------------------- */
  /* read hits                         */

  LXStackp *allHits;
  
  allHits = LXNewStackp(LX_STKP_MIN_SIZE);
  
  if (UtilReadExtendedHits(allHits, seqDictionary, tagDictionary, &param) != LX_NO_ERROR) {
     LX_ERROR("Cannot read hits", 10);
  }
  
  if (param.verbose) 
    fprintf(stderr, "# %ld hits read\n", allHits->top);

  /* --------------------------------- */
  /* sort hits                         */

  if (param.framed) 
    LXSortStackp(allHits, sSortXhitsWithFrame);
  else
    LXSortStackp(allHits, sSortXhitsWithoutFrame);
  
  LXReverseStackp(allHits);
  
  /* --------------------------------- */
  /* make clusters                     */

  if (param.verbose) 
    fprintf(stderr, "# making clusters\n");

  nbClust = sMakeClusters(allHits, &param);

  if (param.verbose) 
    fprintf(stderr, "# %d clusters\n", nbClust);

  /* --------------------------------- */
  /* end                               */

  (void) LXFreeStackp(allHits, UtilFreeXHit);
  
  (void) LXFreeDict(seqDictionary, UtilFreeSeqId);
  (void) LXFreeDict(tagDictionary, UtilFreeTag);

  exit (0);
}

