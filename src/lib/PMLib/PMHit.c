/* -----------------------------------------------------------------------
 * $Id: PMHit.c 1810 2013-02-25 10:04:43Z viari $
 * -----------------------------------------------------------------------
 * @file: PMHit.c
 * @desc: PM hit library
 *
 * @history:
 * @+ <Wanou> : Dec 01 : first version
 * @+ <Gloup> : Mar 06 : code cleanup
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PM hit i/o functions <br>
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXString.h"

#include "PM/PMSys.h"
#include "PM/PMHit.h"

/* ----------------------------------------------------- */
/* @static: decode hit->type                             */
/* ----------------------------------------------------- */

static PMHitType sDecodeType(char code) {
  switch (code) {
    case 'N' :
      return NDefinedHit;
    case 'C' :
      return CDefinedHit;
    case 'F' :
      return FullHit;
  }
  return InvalidHit;
}

/* ----------------------------------------------------- */
/* @static: encode hit->type                             */
/* ----------------------------------------------------- */

static char sEncodeType(PMHitType type) {
  switch (type) {
    case NDefinedHit :
      return 'N';
    case CDefinedHit:
      return 'C';
    case FullHit :
      return 'F';
    default:
      break;
  }
  return '?';
}

/* ----------------------------------------------------- */
/* @static: check hit consistency                        */
/* ----------------------------------------------------- */

static int sCheckHit(PMHitPtr hit) {

  int err = 0;
  
  switch (hit->type) {
    case NDefinedHit :
      if (hit->posMn < 0)
        err++;
      break;
    case CDefinedHit:
      if (hit->posMc < 0)
        err++;
      break;
    case FullHit :
      if ((hit->posMn < 0) || (hit->posMc < 0))
        err++;
      break;
    default:
      err++;
      break;
  }

  return (err ? FIO_ERROR("Inconsistent hit declaration") : LX_NO_ERROR);
}


/* ----------------------------------------------------- */
/* @static: reading hit from buffer                      */
/* ----------------------------------------------------- */

static int sReadFormatHit(char *buffer, PMHitPtr hit) {
  int  nread;
  char type[2];
  char pepSeq[BUFSIZ], pepMatch[BUFSIZ];

  nread = sscanf(buffer, 
                 "%1s%ld%ld%ld%ld%s%s%d%d%d%f",
                 type,
                 &(hit->posMn),
                 &(hit->posMc),
                 &(hit->posSn),
                 &(hit->posSc),
                 pepSeq,
                 pepMatch,
                 &(hit->nbMis),
                 &(hit->nbOver),
                 &(hit->nbMod),
                 &(hit->score));

  if (nread != 11)
    return  IO_ERROR();

  hit->type = sDecodeType(*type);
  
  hit->pepSeq   = LXStr(pepSeq);
  hit->pepMatch = LXStr(pepMatch);
  
  /* --------------------------------------------------- */
  /* convert from bio convention (positions start at 1)  */
  /*           to   C convention (positions start at 0)  */

  hit->posMn--;
  hit->posMc--;
  hit->posSn--;
  hit->posSc--;

  
  return sCheckHit(hit);
}

/* ----------------------------------------------------- */
/* @static: writing hit to stream                        */
/* ----------------------------------------------------- */

static int sWriteFormatHit(FILE *streamou, PMHit *hit) {
  int nwrite;
  char type;

  type = sEncodeType(hit->type);

  /* --------------------------------------------------- */
  /* write in bio convention (positions start at 1)      */
  
  nwrite = fprintf(streamou, 
                   "%c %ld %ld %ld %ld %s %s %d %d %d %f\n",
                   type,
                   hit->posMn + 1,
                   hit->posMc + 1,
                   hit->posSn + 1,
                   hit->posSc + 1,
                   hit->pepSeq,
                   hit->pepMatch,
                   hit->nbMis,
                   hit->nbOver,
                   hit->nbMod,
                   hit->score);
  
  if (nwrite <= 0)
    return IO_ERROR();

  return LX_NO_ERROR;
}

/* -------------------------------------------- */
/* hit allocation                               */
/* -------------------------------------------- */
PMHit *PMNewHit() {
  PMHit *hit;

  if (! (hit = NEW(PMHit))) {
    MEMORY_ERROR();
    return NULL;
  }

  (void) memset(hit, 0, sizeof(PMHit));

  return hit;
}

/* -------------------------------------------- */
/* hit deallocation                             */
/* -------------------------------------------- */
PMHit *PMFreeHit(PMHit *hit) {
  if (hit) {
    if (hit->pepSeq)
      FREE(hit->pepSeq);
    if (hit->pepMatch)
      FREE(hit->pepMatch);
    FREE(hit);
  }

  return NULL;
}

/* -------------------------------------------- */
/* FIO fucntions                                */
/* -------------------------------------------- */

/* -------------------------------------------- */
/* read one hit from buffer                     */
/* -------------------------------------------- */

int PMParseHit(char *buffer, PMHit* hit) {
  return sReadFormatHit(buffer, hit);
}

/* -------------------------------------------- */
/* read hit from file                           */
/* -------------------------------------------- */

int  PMReadHit (FILE *streamin, PMHit *hit) {
  char buffer[BUFSIZ];

   /* get next line in stream */

  if (! LXGetLine(buffer, sizeof(buffer), streamin))
    return LX_EOF;
    
  if (*buffer == PM_END_OF_RECORD)
    return LX_EOF;

  /* parse line */

  return sReadFormatHit(buffer, hit);
}

/* -------------------------------------------- */
/* write hit into file                          */
/* -------------------------------------------- */

int  PMWriteHit(FILE *streamou, PMHit *hit) {
  return sWriteFormatHit(streamou, hit);
}

/* -------------------------------------------- */
/* binary IO (unused)                           */
/* -------------------------------------------- */

int PMBinWriteHit(FILE *streamou, PMHit *hit) {
  long  initpos;
  Int32 val32;
  Int64 val64;
  char  *pepSeq, *pepMatch;

  initpos   = LXTell(streamou);
  
  pepSeq    = (hit->pepSeq   ? hit->pepSeq : "");
  pepMatch  = (hit->pepMatch ? hit->pepMatch : "");

  val32 = (Int32) hit->type;
  if (LXBinWriteInt32(streamou, val32)         != 1) IO_ERROR();
  val64 = (Int64) hit->posMn;
  if (LXBinWriteInt32(streamou, val64)         != 1) IO_ERROR();
  val64 = (Int64) hit->posMc;
  if (LXBinWriteInt32(streamou, val64)         != 1) IO_ERROR();
  val64 = (Int64) hit->posSn;
  if (LXBinWriteInt32(streamou, val64)         != 1) IO_ERROR();
  val64 = (Int64) hit->posSc;
  if (LXBinWriteInt32(streamou, val64)         != 1) IO_ERROR();
  
  if (LXBinWriteStr(streamou, pepSeq)          <= 0) IO_ERROR();
  if (LXBinWriteStr(streamou, pepMatch)        <= 0) IO_ERROR();

  if (LXBinWriteInt32(streamou, hit->nbMis)    != 1) IO_ERROR();
  if (LXBinWriteInt32(streamou, hit->nbOver)   != 1) IO_ERROR();
  if (LXBinWriteInt32(streamou, hit->nbMod)    != 1) IO_ERROR();
  if (LXBinWriteFloat(streamou, hit->score)    != 1) IO_ERROR();
  
  return (int) (LXTell(streamou) - initpos);
}

/* -------------------------------------------- */

int PMBinReadHit (FILE *streamin, PMHit *hit)
{
  long  initpos;
  Int32 val32;
  Int64 val64;
  char  buffer[BUFSIZ];

  initpos = LXTell(streamin);

  if (LXIsEOF(streamin)) return EOF;

  if (LXBinReadInt32(streamin, &val32)           != 1) IO_ERROR();
  hit->type = val32;

  if (LXBinReadInt64(streamin, &val64)          != 1) IO_ERROR();
  hit->posMn = val64;
  if (LXBinReadInt64(streamin, &val64)          != 1) IO_ERROR();
  hit->posMc = val64;
  if (LXBinReadInt64(streamin, &val64)          != 1) IO_ERROR();
  hit->posSn = val64;
  if (LXBinReadInt64(streamin, &val64)          != 1) IO_ERROR();
  hit->posSc = val64;

  if (LXBinReadStr(streamin, buffer)            <= 0) IO_ERROR();
  hit->pepSeq = LXStr(buffer);

  if (LXBinReadStr(streamin, buffer)            <= 0) IO_ERROR();
  hit->pepMatch = LXStr(buffer);
  
  if (LXBinReadInt32(streamin, &(hit->nbMis))   != 1) IO_ERROR();
  if (LXBinReadInt32(streamin, &(hit->nbOver))  != 1) IO_ERROR();
  if (LXBinReadInt32(streamin, &(hit->nbMod))   != 1) IO_ERROR();
  if (LXBinReadFloat(streamin, &(hit->score))   != 1) IO_ERROR();

  (void) sCheckHit(hit);
  
  return (int) (LXTell(streamin) - initpos);
}

/* -------------------------------------------- */
/* print Hit debug information                  */
/* -------------------------------------------- */

void PMDebugHit ( FILE *streamou, PMHit *hit )
{
  fprintf(streamou, "// PMHit\n");
  fprintf(streamou, "// type         : %d (%c)\n", hit->type, sEncodeType(hit->type));
  fprintf(streamou, "// posMn        : %ld\n", hit->posMn);
  fprintf(streamou, "// posMc        : %ld\n", hit->posMc);
  fprintf(streamou, "// posSn        : %ld\n", hit->posSn);
  fprintf(streamou, "// posSc        : %ld\n", hit->posSc);
  fprintf(streamou, "// pepSeq       : %s\n", hit->pepSeq);
  fprintf(streamou, "// pepMatch     : %s\n", hit->pepMatch);
  fprintf(streamou, "// nbMis        : %d\n", hit->nbMis);
  fprintf(streamou, "// nbOver       : %d\n", hit->nbOver);
  fprintf(streamou, "// nbMod        : %d\n", hit->nbMod);
  fprintf(streamou, "// score        : %f\n", hit->score);
  fprintf(streamou, "//\n");
}
