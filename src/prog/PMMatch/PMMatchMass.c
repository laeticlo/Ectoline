/* -----------------------------------------------------------------------
 * $Id: PMMatchMass.c 1773 2013-02-18 00:09:53Z viari $
 * -----------------------------------------------------------------------
 * @file: PMMatchMass.c
 * @desc: PMMatch mass matching functions 
 *
 * @history:
 * @+ <Wanou> : Jan 02 : first version
 * @+ <Gloup> : Aug 06 : complete code revision
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PepMap PMMatch mass matching functions <br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PM.h"

#include "PMMatch.h"

/* ----------------------------------------------- */
/* statics                                         */
/* ----------------------------------------------- */

/* placeholders for mass comparisons */

/* placeholder or verbosity          */

static Bool sVerbose = FALSE;

/* ----------------------------------------------- */
/* recursive check of masses                       */
/* ----------------------------------------------- */

static long sCheckMass  (float           massCumul,
                         float           massMini,
                         float           massMaxi,
                         Bool            isNterm,
                         Bool            lastTry,
                         long            position,
                         int             maxModif,
                         int             *nbModif,
                         char            *peptide,
                         LXFastaSequence *fastaSeq,
                         PMAminoAlphabet *alpha)
{
  long        found;
  int         imod;
  PMAminoAcid *amino;

  /* -------------------------------- */
  /* let's check current position     */
  
  if (massCumul > massMaxi)                 /* too far                             */
    return -1;                              /* not found                           */

  if (    (massCumul >= massMini) 
       && (massCumul <= massMaxi)) {        /* correct mass found                  */
    return position;
  }
  
  if (lastTry)                              /* this is our last try                */
    return -1;                              /* not found                           */

  /* -------------------------------- */
  /* let's check next position        */
      
  position += (isNterm ? -1 : +1);
  
  if (    (position < 0)
       || (position >= fastaSeq->length))
    return -1;                              /* not found                           */
  
  amino = PMGetAminoAcid(alpha, (PMSymbol) fastaSeq->seq[position]);
  
  if (    (amino == NULL)                   /* nothing interesting at this position */
       || (amino->type != PM_AA_ORIGIN))    /* maybe a Stop or invalid code         */
    return -1;                              /* not found                            */
  
  /* -------------------- */
  /* try with original aa */  

  *peptide = amino->oneLetterCode;
  
  if ((found = sCheckMass(massCumul + amino->mass, massMini, massMaxi,
                          isNterm, lastTry, position, maxModif,
                          nbModif, peptide + 1, fastaSeq, alpha)) != -1) {
    return found;
  }

  /* -------------------- */
  /* try with modified aa */  

  if (*nbModif >= maxModif)                  /* too many modifications             */
    return -1;                               /* not found                          */
   
  (*nbModif)++;
  
  for (imod = 0 ; imod < amino->nbMod ; imod++) {
  
    PMAminoAcid *aaMod = PMGetAminoAcid(alpha, amino->modIndx[imod]);

    if (isNterm && (aaMod->modLocation == PM_AA_MOD_LOC_CTERM))
      continue;

    if ((! isNterm) && (aaMod->modLocation == PM_AA_MOD_LOC_NTERM))
      continue;

    lastTry = (aaMod->modLocation != PM_AA_MOD_LOC_ANY);

    *peptide = aaMod->oneLetterCode;
      
    if ((found = sCheckMass(massCumul + aaMod->mass, massMini, massMaxi,
                            isNterm, lastTry, position, maxModif,
                            nbModif, peptide + 1, fastaSeq, alpha)) != -1) {
      return found;
    }
  }

  (*nbModif)--;
  
  return -1;  /* not found */
}

/* ----------------------------------------------- */
/* check enzyme cut sites                          */
/* ----------------------------------------------- */

static Bool sHasCleavage(LXFastaSequence *fastaSeq, long pos, PMEnzyme *enzyme)
{
  int i;
  
  if ((pos < 0) || (pos >= fastaSeq->length))
    return FALSE;
  
  for (i = 0 ; i < enzyme->nbCuts ; i++) {

    PMCutSite cut    = enzyme->cutSite[i];
    Bool      ok     = FALSE;
    long      offset = MAX(pos - cut.NCutOffset, 0);

    char save = fastaSeq->seq[pos];
    fastaSeq->seq[pos] = '\000';

    ok = LXRegExec(cut.NCutRegExp, fastaSeq->seq + offset, TRUE);

    fastaSeq->seq[pos] = save;

    ok = ok && LXRegExec(cut.CCutRegExp, fastaSeq->seq + pos, TRUE);
    
    if (ok)
      return TRUE;
  }
  
  return FALSE;
}

/* ----------------------------------------------- */
/* check if we are putatively at N-term of protein */
/* i.e. begin of sequence or there is a Met in     */
/* the 'metScan' previous characters               */
/* usually metScan is 1  (i.e. check if there is   */
/* a Met atposition 'pos') but Myriam said         */
/* sometimes the Met has been cleaved in the final */
/* protein, so we may check one char before        */
/* (metScan = 2).                                  */
/* note that metScan = 0 will force to never check */
/* for Met.                                        */
/* ----------------------------------------------- */

static Bool sIsNTerm(LXFastaSequence *fastaSeq, long pos, int metScan)
{
  int i;
  
  if (pos <= 0)
    return TRUE; /* begin of sequence   */

  for (i = 0 ; i < metScan ; i++) {
    if ((pos >= 0) && (fastaSeq->seq[pos] == PM_START_CODON_AA))
      return TRUE;
    pos--;
  }

  return FALSE;
}

/* ----------------------------------------------- */
/* check if we are putatively at C-term of protein */
/* ----------------------------------------------- */

static Bool sIsCTerm(LXFastaSequence *fastaSeq, long pos)
{
  if (pos >= fastaSeq->length)
    return TRUE; /* end of sequence */
    
  if (fastaSeq->seq[pos] == PM_STOP_CODON_AA)
    return TRUE; /* Stop on Cterm   */

  return FALSE;
}

/* ----------------------------------------------- */
/* invalidate hit and return FALSE                 */
/* ----------------------------------------------- */

static Bool sInvalid(PMHitPtr hit) 
{
  hit->type = InvalidHit;
  return FALSE;
}


/* ----------------------------------------------- */
/* API                                             */
/* ----------------------------------------------- */

/* ----------------------------------------------- */
/* process single hit                              */
/* check masses and cut sites                      */
/* and setup hit attributes accordingly            */
/* ----------------------------------------------- */

Bool MassMatchHit (PMTag *tag, 
                   PMHit *hit,
                   LXFastaSequence *fastaSeq,
                   PMAminoAlphabet *alpha,
                   PMEnzyme *enzyme,
                   Parameter *param)
{
  long  from, to, pos;
  int   nModif, cModif; 
  float massMini, massMaxi; 
  char  nBuffer[BUFSIZ], cBuffer[BUFSIZ], buffer[BUFSIZ];
  char  nChar, cChar;
  
  sVerbose = param->verbose;

  /* ------------------------------------------ */
  /* check mass on Nterm                        */

  if (param->massTolerance >= 0) {
    massMini = tag->mN - ((tag->mN + tag->mParent) * param->massTolerance);
    massMaxi = tag->mN + ((tag->mN + tag->mParent) * param->massTolerance);
  }
  else {
    massMini = tag->mN + param->massTolerance;
    massMaxi = tag->mN - param->massTolerance;
  }

  nModif = 0;
  hit->posMn = sCheckMass(0.0, massMini, massMaxi, TRUE, FALSE, hit->posSn, 
                          param->maxModif, &nModif, nBuffer, fastaSeq, alpha);

  /* ------------------------------------------ */
  /* check mass on Cterm                        */

  if (param->massTolerance >= 0) {
    massMini = tag->mC - (tag->mC * param->massTolerance);
    massMaxi = tag->mC + (tag->mC * param->massTolerance);
  }
  else {
    massMini = tag->mC + param->massTolerance;
    massMaxi = tag->mC - param->massTolerance;
  }
  
  
  cModif = 0;
  hit->posMc = sCheckMass(0.0, massMini, massMaxi, FALSE, FALSE, hit->posSc, 
                          param->maxModif, &cModif, cBuffer, fastaSeq, alpha);

  /* ------------------------------------------ */
  /* don't insist if both sides don't match     */
  
  if ((hit->posMn < 0) && (hit->posMc < 0))
    return sInvalid(hit);

  /* ------------------------------------------ */
  /* same if too much modifications             */

  hit->nbMod =   (hit->posMn >= 0 ? nModif : 0)
               + (hit->posMc >= 0 ? cModif : 0);

  if (hit->nbMod > param->maxModif)
    return sInvalid(hit);

  /* ------------------------------------------ */
  /* setup hit->type                            */
  
  hit->type = FullHit;

  if (hit->posMn < 0)
    hit->type = CDefinedHit;

  if (hit->posMc < 0)
    hit->type = NDefinedHit;

  if ((! param->partialHit) && (hit->type != FullHit))
    return sInvalid(hit);

  /* ------------------------------------------ */
  /* compute overcleavage and determine hit     */
  /* location type                              */ 
  
  hit->nbOver = 0;
  
  if (    (hit->posMn >= 0)
       && (! sHasCleavage(fastaSeq, hit->posMn, enzyme))
       && (! sIsNTerm(fastaSeq, hit->posMn, param->metScan)))
    hit->nbOver++;
      
  if (    (hit->posMc >= 0)
       && (! sHasCleavage(fastaSeq, hit->posMc + 1, enzyme))
       && (! sIsCTerm(fastaSeq, hit->posMc)))
    hit->nbOver++;

  if (hit->nbOver > param->maxOverCleavage)
    return sInvalid(hit);

  /* ------------------------------------------ */
  /* compute miscleavage                        */

  from = (hit->posMn >= 0 ? hit->posMn + 1 : hit->posSn);
  to   = (hit->posMc >= 0 ? hit->posMc     : hit->posSc);
  
  for (pos = from ; pos <= to ; pos++) {
    if (sHasCleavage(fastaSeq, pos, enzyme))
      hit->nbMis++;
  }

  if (hit->nbMis > param->maxMisCleavage)
    return sInvalid(hit);

  /* ------------------------------------------ */
  /* compute match peptide                      */

  nBuffer[(hit->posMn >= 0) ? hit->posSn - hit->posMn : 0] = '\000';
  cBuffer[(hit->posMc >= 0) ? hit->posMc - hit->posSc : 0] = '\000';

  (void) LXReverseStr(nBuffer);

  (void) sprintf(buffer, "%s[%s]%s", nBuffer, tag->seq, cBuffer);

  hit->pepMatch = LXStr(buffer);
  
  /* ------------------------------------------ */
  /* compute sequence peptide                   */
  
  from = (hit->posMn >= 0 ? hit->posMn : hit->posSn);
  to   = (hit->posMc >= 0 ? hit->posMc : hit->posSc);

  (void) strncpy(nBuffer, fastaSeq->seq + from, to - from + 1);
  nBuffer[to - from + 1] = '\000';
  
  nChar = (((hit->posMn >= 0) && (from > 0)) ? fastaSeq->seq[from - 1] : '*');
  cChar = (((hit->posMc >= 0) && (to < fastaSeq->length - 1)) ? fastaSeq->seq[to + 1] : '*');
  
  (void) sprintf(buffer, "%c.%s.%c", nChar, nBuffer, cChar);
  
  hit->pepSeq = LXStr(buffer);


  return TRUE;
}

/* ----------------------------------------------- */
/* same but with a stack of hits                   */
/* also cleanup stack (to save memory)             */
/* ----------------------------------------------- */

int MassMatchHitStack (PMTag *tag, 
                       PMHitStack *hitStack,
                       LXFastaSequence *fastaSeq,
                       PMAminoAlphabet *alpha,
                       PMEnzyme *enzyme,
                       Parameter *param)
{
  PMHit *hit;
  PMHitStack *tmpStack;
  
  
  if (! (tmpStack = PMNewHitStack()))
    return MEMORY_ERROR();
  
  // filter hits
  
  while (LXPoppOut(hitStack, (Ptr *) &hit)) {
    if (MassMatchHit(tag, hit, fastaSeq, alpha, enzyme, param)) {
      LXPushpIn(tmpStack, (Ptr) hit);
    }
    else {
      PMFreeHit(hit);
    }
  }

  // put stuff back
  
  while (LXPoppOut(tmpStack, (Ptr *) &hit)) {
    LXPushpIn(hitStack, (Ptr) hit);
  }

  // free temporary stack
  
  (void) LXFreeStackp(tmpStack, NULL);

  return LX_NO_ERROR;  
}
