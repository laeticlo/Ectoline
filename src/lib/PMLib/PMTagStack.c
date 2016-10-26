/* -----------------------------------------------------------------------
 * $Id: PMTagStack.c 993 2010-04-14 22:51:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMTagStack.c
 * @desc: stack of tags management
 *
 * @history:
 * @+ <romain>  : Jan 03 : first version
 * @+ <Gloup>   : Dec 05 : major revision
 * @+ <Gloup>   : Feb 06 : code cleanup
 * @+ <Gloup>   : Aug 06 : moved from TGlib to PMLib
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * stack of tags management library<br>
 * specialization of LXStkp
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>

#include "PM/PMTagStack.h"

/* ---------------------------------------------------- */
/* Statics                                              */
/* predefined comparison functions                      */
/* ---------------------------------------------------- */

static int sTagCompId(const void *apt1, const void *apt2)
{
  int res;
  PMTag *pt1, *pt2;
  
  pt1 = *((PMTag**) apt1);
  pt2 = *((PMTag**) apt2);
  
  return strcmp(pt1->id, pt2->id);
}

static int sTagCompSeq(const void *apt1, const void *apt2)
{
  PMTag *pt1, *pt2;

  pt1 = *((PMTag**) apt1);
  pt2 = *((PMTag**) apt2);
  
  return strcmp(pt1->seq, pt2->seq);
}

static int sTagCompMn(const void *apt1, const void *apt2)
{
  PMTag *pt1, *pt2;

  pt1 = *((PMTag**) apt1);
  pt2 = *((PMTag**) apt2);

  if (pt1->mN == pt2->mN)
    return 0;
    
  return ((pt1->mN < pt2->mN) ? -1 : 1);
}


static int sTagCompMc(const void *apt1, const void *apt2)
{
  int res;
  PMTag *pt1, *pt2;

  pt1 = *((PMTag**) apt1);
  pt2 = *((PMTag**) apt2);

  if (pt1->mC == pt2->mC)
    return 0;
    
  return ((pt1->mC < pt2->mC) ? -1 : 1);
}

static int sTagCompScore(const void *apt1, const void *apt2)
{
  int res;
  PMTag *pt1, *pt2;

  pt1 = *((PMTag**) apt1);
  pt2 = *((PMTag**) apt2);

  if (pt1->score == pt2->score)
    return 0;
    
  return ((pt1->score < pt2->score) ? -1 : 1);
}

/* ---------------------------------------------------- */
/* Statics                                              */
/* predefined free function                             */
/* ---------------------------------------------------- */

static void sFreeTag(void *apt)
{
  PMFreeTag((PMTag *) apt);
}

/* ==================================================== */
/* PMTagStack API                                       */
/* ==================================================== */

/*--------------------------------------------------*/
/* create PMTagStack                                */
/*--------------------------------------------------*/

PMTagStack *PMNewTagStack()
{
  PMTagStack *stkp;
  
  if (! (stkp = LXNewStackp(LX_STKP_MIN_SIZE))) {
      MEMORY_ERROR();
  }
  
  return stkp;
}

/*--------------------------------------------------*/
/* free PMTagStack                                  */
/*--------------------------------------------------*/

PMTagStack *PMFreeTagStack(PMTagStack *stkp)
{
  return LXFreeStackp(stkp, sFreeTag);
}

/*--------------------------------------------------*/
/* sort PMTagStack                                  */
/*--------------------------------------------------*/

void PMSSortTagStack(PMTagStack *stkp, PMTagSortType sortBy)
{
  switch (sortBy) {
    case sortTagsById : 
      LXSortStackp(stkp, sTagCompId);
      break;
    case sortTagsBySeq : 
      LXSortStackp(stkp, sTagCompSeq);
      break;
    case sortTagsByMn : 
      LXSortStackp(stkp, sTagCompMn);
      break;
    case sortTagsByMc : 
      LXSortStackp(stkp, sTagCompMc);
      break;
    case sortTagsByScore :
      LXSortStackp(stkp, sTagCompScore);
      break;
    default:
      ARG_ERROR();
    }
}

/*--------------------------------------------------*/
/* Read TagStack from file                          */
/*--------------------------------------------------*/

int PMReadTagStack(FILE *streamin, PMTagStack *stkp, Bool skipMark)
{
  PMTag *tag;
  char  buffer[BUFSIZ];
  
  while (LXGetLine(buffer, sizeof(buffer), streamin)) {

    int res; 
    
    if (*buffer == PM_END_OF_RECORD) {
      if (skipMark)
        continue;
      return LX_NO_ERROR;
    }

    if (! (tag = PMNewTag()))
      return MEMORY_ERROR();
  
    if ((res = PMParseTag(buffer, tag)) != LX_NO_ERROR) {
      return res;
    }
      
    if ( ! LXPushpIn(stkp, tag))
      return MEMORY_ERROR();
    
  }

  return LX_NO_ERROR;
}


/* -------------------------------------------- */
/* Load TagStack from file                      */
/* -------------------------------------------- */

int PMLoadTagStack(char *filename, PMTagStack *stkp, Bool skipMark)
{
  FILE *tagFile;
 
  if (! (tagFile = LXOpenFile(filename, "r")))
    return IO_ERROR();

  int res = PMReadTagStack(tagFile, stkp, skipMark);
  
  (void) fclose(tagFile);

  return res;
}

/*--------------------------------------------------*/
/* Write TagStack to file                           */
/*--------------------------------------------------*/

int PMWriteTagStack(FILE *streamou, PMTagStack *stkp)
{
  long i;
  int  res = LX_NO_ERROR;
  
  for (i = 0 ; i < stkp->top ; i++ ) {
  
    PMTag *tag = (PMTag*) stkp->val[i];
    
    if ((res = PMWriteTag(streamou, tag)) != LX_NO_ERROR)
      break;
  }
  
  if (res == LX_NO_ERROR) {
    (void) fprintf(streamou, "%cEndOfTags\n", PM_END_OF_RECORD);
  }

  return res;
}
