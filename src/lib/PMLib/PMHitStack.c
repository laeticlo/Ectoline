/* -----------------------------------------------------------------------
 * $Id: PMHitStack.c 1810 2013-02-25 10:04:43Z viari $
 * -----------------------------------------------------------------------
 * @file: PMHitStack.c
 * @desc: stack of hits management
 *
 * @history:
 * @+ <Gloup>   : Aug 06 : first version
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * stack of hits management library<br>
 * specialization of LXStackp
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>

#include "PM/PMHit.h"
#include "PM/PMHitStack.h"

/* ---------------------------------------------------- */
/* Statics                                              */
/* predefined comparison functions                      */
/* ---------------------------------------------------- */

static int sHitCompPosMn(const void *apt1, const void *apt2)
{
  const PMHit *pt1, *pt2;

  pt1 = *((const PMHit**) apt1);
  pt2 = *((const PMHit**) apt2);

  if (pt1->posMn == pt2->posMn)
    return 0;
    
  return ((pt1->posMn < pt2->posMn) ? -1 : 1);
}

static int sHitCompPosSn(const void *apt1, const void *apt2)
{
  const PMHit *pt1, *pt2;

  pt1 = *((const PMHit**) apt1);
  pt2 = *((const PMHit**) apt2);

  if (pt1->posSn == pt2->posSn)
    return 0;
    
  return ((pt1->posSn < pt2->posSn) ? -1 : 1);
}

static int sHitCompScore(const void *apt1, const void *apt2)
{
  const PMHit *pt1, *pt2;

  pt1 = *((const PMHit**) apt1);
  pt2 = *((const PMHit**) apt2);

  if (pt1->score == pt2->score)
    return 0;
    
  return ((pt1->score < pt2->score) ? -1 : 1);
}

/* ---------------------------------------------------- */
/* Statics                                              */
/* predefined free function                             */
/* ---------------------------------------------------- */

static void sFreeHit(void *apt)
{
  PMFreeHit((PMHit *) apt);
}

/* ==================================================== */
/* PMHitStack API                                       */
/* ==================================================== */

/*--------------------------------------------------*/
/* create PMHitStack                                */
/*--------------------------------------------------*/

PMHitStack *PMNewHitStack(void)
{
  PMHitStack *stkp;
  
  if (! (stkp = LXNewStackp(LX_STKP_MIN_SIZE))) {
      MEMORY_ERROR();
  }
  
  return stkp;
}

/*--------------------------------------------------*/
/* free PMHitStack                                  */
/*--------------------------------------------------*/

PMHitStack *PMFreeHitStack(PMHitStack *stkp)
{
  return LXFreeStackp(stkp, sFreeHit);
}

/*--------------------------------------------------*/
/* sort PMHitStack                                  */
/*--------------------------------------------------*/

void PMSortHitStack(PMHitStack *stkp, PMHitSortType sortBy)
{
  switch (sortBy) {
    case sortHitsByPosMn : 
      LXSortStackp(stkp, sHitCompPosMn);
      break;
    case sortHitsByPosSn : 
      LXSortStackp(stkp, sHitCompPosSn);
      break;
    case sortHitsByScore : 
      LXSortStackp(stkp, sHitCompScore);
      break;
    default:
      ARG_ERROR();
    }
}

/*--------------------------------------------------*/
/* Read HitStack from file                          */
/*--------------------------------------------------*/

int PMReadHitStack(FILE *streamin, PMHitStack *stkp, Bool skipMark)
{
  PMHit *hit;
  char  buffer[BUFSIZ];
  
  while (LXGetLine(buffer, sizeof(buffer), streamin)) {

    int res; 

    if (*buffer == PM_END_OF_RECORD) {
      if (skipMark)
        continue;
      return LX_NO_ERROR;
    }
      
    if (! (hit = PMNewHit()))
      return MEMORY_ERROR();
  
    if ((res = PMParseHit(buffer, hit)) != LX_NO_ERROR) {
      return res;
    }

    if ( ! LXPushpIn(stkp, hit))
      return MEMORY_ERROR();
    
  }

  return LX_NO_ERROR;
}


/* -------------------------------------------- */
/* Load HitStack from file                      */
/* -------------------------------------------- */

int PMLoadHitStack(char *filename, PMHitStack *stkp, Bool skipMark)
{
  FILE *hitFile;
 
  if (! (hitFile = LXOpenFile(filename, "r")))
    return IO_ERROR();

  int res = PMReadHitStack(hitFile, stkp, skipMark);
  
  (void) fclose(hitFile);

  return res;
}

/*--------------------------------------------------*/
/* Write HitStack to file                           */
/*--------------------------------------------------*/

int PMWriteHitStack(FILE *streamou, PMHitStack *stkp)
{
  size_t i;
  int  res = LX_NO_ERROR;
  
  for (i = 0 ; i < stkp->top ; i++ ) {
  
    PMHit *hit = (PMHit*) stkp->val[i];
    
    if ((res = PMWriteHit(streamou, hit)) != LX_NO_ERROR)
      break;
  }
  
  if (res == LX_NO_ERROR) {
    (void) fprintf(streamou, "%cEndOfHits\n", PM_END_OF_RECORD);
  }

  return res;
}
