/* -----------------------------------------------------------------------
 * $Id: PMTag.c 1810 2013-02-25 10:04:43Z viari $
 * -----------------------------------------------------------------------
 * @file: PMTag.c
 * @desc: PM tag library
 *
 * @history:
 * @+ <Wanou> : Dec 01 : first version
 * @+ <Wanou> : Feb 02 : new GEX format
 * @+ <Wanou> : May 02 : comment feature added
 * @+ <Gloup> : Aug 05 : removed GEX/LX formats
 * @+ <Gloup> : Jul 06 : code cleanup
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PM tag i/o functions <br>
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PM/PMSys.h"
#include "PM/PMTag.h"

/* ----------------------------------------------------- */
/* @static: read Tag - New format                        */
/* ----------------------------------------------------- */

static int sReadFormatTag(char *buffer, PMTag *tag)
{
  int nread;

  nread = sscanf(buffer, "%s%f%f%s%f%f",
                         tag->id,
                         &(tag->mN),
                         &(tag->mC),
                         tag->seq,
                         &(tag->mParent),
                         &(tag->score));

  if (nread != 6) {
    fprintf(stderr, "// IO-Format error at: %s\n", buffer);
    return IO_ERROR();
  }

  return LX_NO_ERROR; 
} 

/* ----------------------------------------------------- */
/* @static: write Tag - New format                       */
/* ----------------------------------------------------- */

static int sWriteFormatTag(FILE *streamou, PMTag *tag)
{
  int nwrite;

  nwrite= fprintf(streamou, "%s %f %f %s %f %g\n",
                            tag->id,
                            tag->mN,
                            tag->mC,
                            tag->seq,
                            tag->mParent,
                            tag->score);

  return ((nwrite <= 0) ? IO_ERROR() : LX_NO_ERROR);
} 

/* -------------------------------------------- */
/* API                                          */
/* -------------------------------------------- */

/* -------------------------------------------- */
/* -------------------------------------------- */

/* -------------------------------------------- */
/* tag allocation                               */
/* -------------------------------------------- */
PMTag *PMNewTag ()
{
  PMTag *tag;
        
  if (! (tag = NEW(PMTag))) {
    MEMORY_ERROR();
    return NULL;
  }

  memset(tag, 0, sizeof(PMTag));

  return tag;
}

/* -------------------------------------------- */
/* tag deallocation                             */
/* -------------------------------------------- */
PMTag *PMFreeTag (PMTag *tag)
{
  if (tag) {
    FREE(tag);
  }

  return NULL;
}

/* -------------------------------------------- */
/* FIO funcs                                    */
/* -------------------------------------------- */

/* -------------------------------------------- */
/* read one tag from buffer                     */
/* -------------------------------------------- */

int PMParseTag(char *buffer, PMTag* tag)
{
  return sReadFormatTag(buffer, tag);
}

/* -------------------------------------------- */
/* read one tag                                 */
/* -------------------------------------------- */

int PMReadTag(FILE *streamin, PMTag *tag)
{
  char buffer[BUFSIZ];

   /* get next line in stream */

  if (! LXGetLine(buffer, sizeof(buffer), streamin))
      return LX_EOF;

  if (*buffer == PM_END_OF_RECORD)
    return LX_EOF;

  /* parse line */

  return sReadFormatTag(buffer, tag);
}

/* -------------------------------------------- */
/* write one tag                                */
/* -------------------------------------------- */

int PMWriteTag(FILE *streamou, PMTag *tag)
{
  return sWriteFormatTag(streamou, tag);
}


/* -------------------------------------------- */
/* print Tag debug information                  */
/* -------------------------------------------- */

void PMDebugTag ( FILE *streamou, PMTag *tag )
{
  fprintf(streamou, "// PMTag\n");
  fprintf(streamou, "// id           : %s\n", tag->id);
  fprintf(streamou, "// seq          : %s\n", tag->seq);
  fprintf(streamou, "// mN           : %f\n", tag->mN);
  fprintf(streamou, "// mC           : %f\n", tag->mC);
  fprintf(streamou, "// mParent      : %f\n", tag->mParent);
  fprintf(streamou, "// score        : %f\n", tag->score);
  fprintf(streamou, "//\n");
}

/* -------------------------------------------- */

int PMBinWriteTag (FILE *streamou, PMTag *tag)
{
  int res;
  Int32 initpos;

  initpos = LXTell(streamou);

  if ((res=LXBinWriteChars(streamou, tag->id, PM_TAG_TAGID_LEN + 2)) <= 0) {
    return -IO_ERROR();
  }

  if ((res=LXBinWriteChars(streamou, tag->seq, PM_TAG_MAX_SEQ_LEN + 2)) <= 0) {
    return -IO_ERROR();
  }

  if ((res=LXBinWriteFloat(streamou, tag->mN)) != 1) {
    return -IO_ERROR();
  }

  if ((res=LXBinWriteFloat(streamou, tag->mC)) != 1) {
    return -IO_ERROR();
  }

  if ((res=LXBinWriteFloat(streamou, tag->mParent)) != 1) {
    return -IO_ERROR();
  }

  if ((res=LXBinWriteFloat(streamou, tag->score)) != 1) {
    return -IO_ERROR();
  }

  return (int) (LXTell(streamou) - initpos);
}

/* -------------------------------------------- */

int PMBinReadTag (FILE *streamin, PMTag *tag)
{
  int res;
  Int32 initpos;

  initpos = LXTell(streamin);

  if (LXIsEOF(streamin)) {
    return LX_EOF;
  }
  
  if ((res=LXBinReadChars(streamin, tag->id, PM_TAG_TAGID_LEN + 2)) <= 0) {
    return -IO_ERROR();
  }

  if ((res=LXBinReadChars(streamin, tag->seq, PM_TAG_MAX_SEQ_LEN + 2)) <= 0) {
    return -IO_ERROR();
  }

  if ((res=LXBinReadFloat(streamin, &(tag->mN))) != 1) {
    return -IO_ERROR();
  }

  if ((res=LXBinReadFloat(streamin, &(tag->mC))) != 1) {
    return -IO_ERROR();
  }

  if ((res=LXBinReadFloat(streamin, &(tag->mParent))) != 1) {
    return -IO_ERROR();
  }

  if ((res=LXBinReadFloat(streamin, &(tag->score))) != 1) {
    return -IO_ERROR();
  }

  return (int) (LXTell(streamin) - initpos);
}
