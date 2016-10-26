/* -----------------------------------------------------------------------
 * $Id: PMMatchUtil.c 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMMatchUtil.c
 * @desc: PMMatch utilities functions 
 *
 * @history:
 * @+ <Wanou> : Jul 02 : first version
 * @+ <Gloup> : Aug 06 : complete code revision
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PepMap PMMatch utilities functions<br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX.h"
#include "PM.h"

#include "PMMatch.h"

/* ----------------------------------------------- */
/* make sure sequence is composed only of upper    */
/* characters                                      */
/* ----------------------------------------------- */

char *UtilValidateSequence (char *seq)
{
  char *s;
  
  for (s = seq ; *s ; s++) {

    if (! isalpha(*s))
      *s = LX_GENETIC_BAD_AA;

    if (islower(*s))
      *s = toupper(*s);
  }
  
  return seq;
}

/* ----------------------------------------------------- */
/* check that all tags in TagStack have the same length  */
/* ----------------------------------------------------- */

int UtilCheckTags(PMTagStack *tagStack)
{
  long i, n;
  int  kuple;

  kuple = 0;
  
  n = LXStackpSize(tagStack);
  
  for (i = 0 ; i < n ; i++) {
  
    PMTag *tag = (PMTag *) LXStackpGetValue(tagStack, i);
    
    (void) UtilValidateSequence(tag->seq);

    if (i == 0)
      kuple = strlen(tag->seq);
    else if (kuple != strlen(tag->seq))
      return 0;
   }
   
   return kuple;
}

/* ----------------------------------------------------- */
/* print stack of hits associated to tag                 */
/* ----------------------------------------------------- */

int UtilPrintHitStack(PMTag *tag, PMHitStack *hitStack, LXFastaSequence *fastaSeq)
{
  PMSeqId seqId;
  
  if (LXStackpSize(hitStack) == 0)
    return LX_NO_ERROR;

#if 0
  if (PMFasta2SeqId(fastaSeq, &seqId) != LX_NO_ERROR)
    return ERROR("Invalid Sequence header", LX_ARG_ERROR);
#else
  // [AV] this is for use with unformated proteic sequences
  (void) PMFasta2SeqIdSafe(fastaSeq, &seqId);
#endif

  if (PMWriteSeqId(stdout, &seqId) != LX_NO_ERROR)
    return IO_ERROR();

  if (PMWriteTag(stdout, tag) != LX_NO_ERROR)
    return IO_ERROR();
    
  if (PMWriteHitStack(stdout, hitStack) != LX_NO_ERROR)  
    return IO_ERROR();
    
  return LX_NO_ERROR;
}


