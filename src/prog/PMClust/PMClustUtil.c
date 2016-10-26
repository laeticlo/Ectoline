/* -----------------------------------------------------------------------
 * $Id: PMClustUtil.c 1112 2010-05-23 18:01:09Z viari $
 * -----------------------------------------------------------------------
 * @file: PMClustUtil.c
 * @desc: PMClust utilities functions 
 *
 * @history:
 * @+ <Wanou> : Jul 02 : first version
 * @+ <Gloup> : Aug 06 : complete code revision
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PepMap PMClust utilities functions<br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX.h"
#include "PM.h"

#include "PMClust.h"

/* ----------------------------------------------- */
/* compare SeqId                                   */
/*                                                 */
/* we perform comparison from cheapest to most     */
/* expensive                                       */
/* ----------------------------------------------- */

int UtilCompareSeqId(const void *p1, const void *p2)
{
  PMSeqId *s1 = (PMSeqId *) p1;
  PMSeqId *s2 = (PMSeqId *) p2;

  int cmp;
 
  if ((cmp = (s1->frame - s2->frame)) != 0)
    return cmp;

  if ((cmp = (s1->strand - s2->strand)) != 0)
    return cmp;

  if ((cmp = strcmp(s1->name, s2->name)) != 0)
    return cmp;
 
    //
    // [JET] in genoproteo project, the comment is used to pass information
    // between PMTrans and PMClust.
    // So we need to differenciate sequences on their comments.
    // [AV] ca ne me gene pas de tester aussi le commentaire
    //

  if ((cmp = strcmp(s1->comment, s2->comment)) != 0)
    return cmp;
    
  return 0;
}

/* ----------------------------------------------- */
/* free SeqId                                      */
/* ----------------------------------------------- */

void UtilFreeSeqId(void *p)
{
  PMSeqId *s = (PMSeqId *) p;
  if (s) FREE(s);
}

/* ----------------------------------------------- */
/* compare Tags                                    */
/*                                                 */
/* we perform comparison from cheapest to most     */
/* expensive                                       */
/* ----------------------------------------------- */

int UtilCompareTag(const void *p1, const void *p2)
{
  PMTag *t1 = (PMTag *) p1;
  PMTag *t2 = (PMTag *) p2;

  int cmp;
  
  if (t1->mN != t2->mN)
    return (t1->mN > t2->mN ? 1 : -1);

  if (t1->mC != t2->mC)
    return (t1->mC > t2->mC ? 1 : -1);

  if (t1->mParent != t2->mParent)
    return (t1->mParent > t2->mParent ? 1 : -1);

  if (t1->score != t2->score)
    return (t1->score > t2->score ? 1 : -1);

  if ((cmp = strcmp(t1->seq, t2->seq)) != 0)
    return cmp;
    
  return strcmp(t1->id, t2->id);
}

/* ----------------------------------------------- */
/* free Tag                                        */
/* ----------------------------------------------- */

void UtilFreeTag(void *p)
{
  PMTag *t = (PMTag *) p;
  
  if (t) (void) PMFreeTag(t);
}

/* ----------------------------------------------- */
/* compute projected nucleotidic position          */
/* ----------------------------------------------- */

void UtilNucleotidicPosition(PMSeqId *seqId, long nPos, long cPos, long *from, long *to)
{
  long nnPos = (3 * nPos) + seqId->frame - 1;
  long ccPos = (3 * cPos) + seqId->frame + 1;
  
  if (seqId->strand == PM_SEQID_DIRECT) {
    *from = (nPos >= 0 ? nnPos : nPos);
    *to   = (cPos >= 0 ? ccPos : cPos);
  }
  else {
    *from = (cPos >= 0 ? (seqId->length - 1) - ccPos : cPos);
    *to   = (nPos >= 0 ? (seqId->length - 1) - nnPos : nPos);
  }
}

/* ----------------------------------------------- */
/* new extended hit                                */
/* ----------------------------------------------- */

ExtendedHit *UtilNewXHit(PMSeqId *seqId, PMTag *tag , PMHit *hit, Bool isProteic)
{
  long nPos, cPos;
  ExtendedHit *xhit;
  
  if (! (xhit = NEW(ExtendedHit))) {
    MEMORY_ERROR();
    return NULL;
  }
  
  xhit->seqId = seqId;
  xhit->tag   = tag;
  xhit->hit   = hit;

  nPos = ((xhit->hit->type == CDefinedHit) ? xhit->hit->posSn : xhit->hit->posMn);
  cPos = ((xhit->hit->type == NDefinedHit) ? xhit->hit->posSc : xhit->hit->posMc);

  if (isProteic) {
    xhit->from = nPos;
    xhit->to   = cPos;
  }
  else {
    UtilNucleotidicPosition(xhit->seqId, nPos, cPos, &(xhit->from), &(xhit->to));
  }
  
  return xhit;
}

/* ----------------------------------------------- */
/* free extended hit                               */
/* just free hit , not seqId nor tags that are     */
/* shared                                          */
/* ----------------------------------------------- */

void UtilFreeXHit(void *data) {
  ExtendedHit *xhit = (ExtendedHit *) data;
  if ((xhit) && (xhit->hit))
    (void) PMFreeHit(xhit->hit);
}

/* ----------------------------------------------- */
/* read hits and push extended hits into stkp      */
/* ----------------------------------------------- */

int UtilReadExtendedHits(LXStackp *stkp, LXDict *seqDict, LXDict *tagDict,
                         Parameter *param)
{
  int stat;
  
	PMHitStack *hits;
	
	hits = LXNewStackp(LX_STKP_MIN_SIZE);

	while (1) {

	  /* --------------------------------------- */
	  /*  read SeqId                             */
	  
		PMSeqId seqId, *currentId;
	
		stat = PMReadSeqId(stdin, &seqId);
	
	    if (stat == LX_EOF) {
	        stat = LX_NO_ERROR;
	        break;
	    }

		if (stat != LX_NO_ERROR)
	        break;
		
		if (LXHasDict(seqDict, &seqId)) {
			currentId = (PMSeqId *) LXGetDict(seqDict, &seqId);
		}
		else {
			currentId = NEW(PMSeqId);
			(void) memcpy(currentId, &seqId, sizeof(PMSeqId));
			(void) LXAddDict(seqDict, currentId);
		}

	  /* --------------------------------------- */
	  /*  read Tag                               */

        PMTag tag, *currentTag;
    
		stat = PMReadTag(stdin, &tag);

		if (stat != LX_NO_ERROR)
	    break;

		if (LXHasDict(tagDict, &tag)) {
			currentTag = (PMTag *) LXGetDict(tagDict, &tag);
		}
		else {
			currentTag = PMNewTag();
			(void) memcpy(currentTag, &tag, sizeof(PMTag));
			(void) LXAddDict(tagDict, currentTag);
		}

	  /* --------------------------------------- */
	  /*  read Hits and build extended hits      */
		
		PMHit *hit;
		
		stat = PMReadHitStack(stdin, hits, FALSE);
		
		if (stat != LX_NO_ERROR)
	    break;
	
		while (LXPoppOut(hits, (Ptr *) &hit)) {
		  ExtendedHit *xhit = UtilNewXHit(currentId, currentTag, hit, param->isProteic);
		  (void) LXPushpIn(stkp, (Ptr) xhit);
		}
		
	}

	(void) LXFreeStackp(hits, NULL);

  return stat;
}
