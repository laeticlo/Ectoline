/* -----------------------------------------------------------------------
 * $Id: LXFastq.c 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: LXFastq.c
 * @desc: sequence IO in fastq format
 * 
 * @history:
 * @+ <Gloup> : Aug 92 : first version
 * @+ <Gloup> : Nov 95 : last revised version
 * @+ <Wanou> : Dec 01 : adapted to LX_Ware 
 * @+ <Gloup> : Feb 02 : corrected comment bug 
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * @+ <Gloup> : Jan 13 : added nonSanger formats
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LXFastq C program<br>
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "LX/LXString.h"
#include "LX/LXFastq.h"

#define READ_NEXT FALSE
#define PUSH_BACK TRUE

#define SERIAL    TRUE
#define INDEXED   FALSE

#define LINE_FEED '\n'

#ifndef SEEK_SET        /* undefined on SUNOS 4.x */
#define SEEK_SET        0
#define SEEK_CUR        1
#define SEEK_END        2
#endif

/* -------------------------------------------- */
/* @static: read line from streamin             */
/* -------------------------------------------- */
static char *sReadLine(FILE *streamin, Bool pushback, Bool serial)
{
    static String sBuf = NULL;
    static Bool sPushed = FALSE;

    Bool ok = TRUE;
    
    if (pushback) {
      sPushed = TRUE;
    }
    else if (sPushed && serial) {
      sPushed = FALSE;
    }
    else {
      sPushed = FALSE;
      ok = (LXReadStr(streamin, &sBuf) >= 0);
    }
    
    return (ok ? sBuf : NULL);
}

/* -------------------------------------------------- */
/* @static: make sure sequence buffer is large enough */
/*          or double buffer size                     */
/* -------------------------------------------------- */

static Bool sAssertSeqBuffer(LXFastqSequencePtr seq, size_t size) {
    if (size >= LXAllocSize(seq->seq)) {
        size = 2 * size + 1;
        char *sbuf = REALLOC(seq->seq, char, size);
        if (! sbuf) return FALSE;
        unsigned char *qbuf = REALLOC(seq->qual, unsigned char, size);
        if (! qbuf) {
          FREE(sbuf);
          return FALSE;
        }
        seq->seq  = sbuf;
        seq->qual = qbuf;
    }
    return TRUE;
}

/* -------------------------------------------- */
/*  @static: decode quality                     */
/* -------------------------------------------- */

static void sDecodeQuality(LXFastqSequencePtr seq, LXFastqEncoding code) {
    long i;
    int  off;
    
    switch (code) {
      case LX_FASTQ_SANGER   : off = 33 ; break;
      case LX_FASTQ_ILLUMINA : off = 64 ; break;
      case LX_FASTQ_SOLEXA   : off = 64 ; break;
      default                : off = 33 ;
    }

    for (i = 0 ; i < seq->length ; i++) {
      seq->qual[i] = seq->qual[i] - off;
    }
  
    if (code == LX_FASTQ_SOLEXA) {
      double alpha = 10. / log(10.);
      for (i = 0 ; i < seq->length ; i++) {
        double q = alpha * log(1.0+exp(((double) seq->qual[i])/alpha));
        seq->qual[i] = (unsigned char) q;
      }
    }
}

/* -------------------------------------------- */
/*  @static: encode quality in Sanger format    */
/* -------------------------------------------- */

static void sEncodeQuality(LXFastqSequencePtr seq) {
    long i;
    for (i = 0 ; i < seq->length ; i++) {
      seq->qual[i] = seq->qual[i] + 33;
    }
}

/* -------------------------------------------- */
/* @static: skip to next space in buffer        */
/* -------------------------------------------- */
static char *sNextSpace(char *buffer)
{
    for (; *buffer ; buffer++)
       if (isspace(*buffer))
            return buffer;
    
    return NULL;
}

/* -------------------------------------------- */
/* returns sequence name (FASTQ)                */
/* -------------------------------------------- */

static char *sGetName(char *buffer)
{
    static char sName[BUFSIZ];

    if (sscanf(buffer + 1, "%s", sName) != 1)
        (void) strcpy(sName, "NO_NAME");

    return sName;
}

/* -------------------------------------------- */
/* returns sequence comment (FASTQ)             */
/* -------------------------------------------- */
static char *sGetComment(char *buffer)
{
    char   *space;

    static char sComment[BUFSIZ];

    space = sNextSpace(buffer);
    
    strcpy(sComment, (space ? space + 1 : ""));

    return sComment;
}

/* -------------------------------------------- */
/* free Fastq sequence                          */
/* -------------------------------------------- */
LXFastqSequencePtr LXFastqFreeSequence(LXFastqSequencePtr seq)
{
    if (seq) {
        IFFREE(seq->name);
        IFFREE(seq->comment);
        IFFREE(seq->seq);
        IFFREE(seq->qual);
        FREE(seq);
    }

    return NULL;
}
        
/* -------------------------------------------- */
/* create new Fastq sequence                    */
/* -------------------------------------------- */
LXFastqSequencePtr LXFastqNewSequence(long bufsize)
{
    LXFastqSequencePtr seq;
    
    if (! (seq = NEW(LXFastqSequence)))
        return NULL;
       
    seq->length   = 0;
    seq->offset   = -1;
    seq->name     = NULL;
    seq->comment  = NULL;
    seq->seq      = NULL;
    seq->qual     = NULL;
    
    bufsize = (bufsize <= 0 ? LX_FASTQ_MIN_BUFSIZ : bufsize);

    if (! (seq->seq = NEWN(char,  bufsize)))
        return LXFastqFreeSequence(seq);
    if (! (seq->qual = NEWN(unsigned char,  bufsize)))
        return LXFastqFreeSequence(seq);

    seq->ok = TRUE;
    
    return seq;
}

/* -------------------------------------------- */
/* Serial read Fastq sequence                   */
/* returns : FALSE -> last sequence             */
/*           TRUE  -> more to read              */
/*           <but> you must check seq->ok !     */
/*           that may be FALSE (memory error)   */
/* -------------------------------------------- */
Bool LXFastqReadSequence(FILE *streamin, LXFastqSequencePtr seq, LXFastqEncoding code)
{
    long  buflen;
    char  *buffer;

    seq->ok = FALSE;                        /* assume error         */

    buflen = seq->length = 0; 
    
    seq->offset = ftell(streamin);

    buffer = sReadLine(streamin, READ_NEXT, SERIAL);

    if (   (buffer == NULL) 
        || (*buffer != LX_FASTQ_SEQ_BEGIN)) /* sync error           */
        return FALSE;                       /* last sequence        */
    
    if (seq->offset)
        seq->offset -= (strlen(buffer) + 1);

    seq->name    = LXStrcpy(seq->name,    sGetName(buffer));
    seq->comment = LXStrcpy(seq->comment, sGetComment(buffer));

    /* --------------------------
     * read sequence part
     */
     
    while ((buffer = sReadLine(streamin, READ_NEXT, SERIAL)) != NULL) {

        if (*buffer == LX_FASTQ_QUAL_BEGIN)
            break;
    
        if (*buffer == LX_FASTQ_SEQ_BEGIN) {
            (void) sReadLine(streamin, PUSH_BACK, SERIAL);
            break;
        }

        long readlen = strlen(buffer);
    
        buflen += readlen;
   
        if (! sAssertSeqBuffer(seq, buflen))
          return TRUE;   /* but seq->ok is FALSE  */
          
        (void) memcpy(seq->seq + seq->length, buffer, readlen);

        seq->length = buflen;
    }

    seq->seq[seq->length] = '\000';
    
    if ((! buffer) || (*buffer != LX_FASTQ_QUAL_BEGIN))
      return TRUE;   /* but seq->ok is FALSE  */

    /* --------------------------
     * read quality part
     */

    long quallen = 0;

    while ((buffer = sReadLine(streamin, READ_NEXT, SERIAL)) != NULL) {

        if ((*buffer == LX_FASTQ_SEQ_BEGIN) && (quallen >= buflen)) {
            (void) sReadLine(streamin, PUSH_BACK, SERIAL);
            break;
        }

        long readlen = strlen(buffer);
    
        quallen += readlen;
        
        if (quallen <= buflen)
            (void) memcpy(seq->qual + quallen - readlen, buffer, readlen);
            
    }

    seq->qual[seq->length] = '\000';

    /* --------------------------
     * decode quality
     */

    sDecodeQuality(seq, code);

    return (seq->ok = TRUE);
}

/* -------------------------------------------- */
/* Indexed read Fastq sequence                  */
/* returns : FALSE -> memory error              */
/*           TRUE  -> read ok                   */
/* -------------------------------------------- */
Bool LXFastqGetSequence(FILE *streamin, LXFastqSequencePtr seq, LXFastqEncoding code)
{
    long  buflen;
    char  *buffer;

    seq->ok = FALSE;                        /* assume error         */

    buflen = seq->length = 0; 
    
    fseek(streamin, seq->offset, SEEK_SET);

    buffer = sReadLine(streamin, READ_NEXT, INDEXED);

    if (   (buffer == NULL) 
        || (*buffer != LX_FASTQ_SEQ_BEGIN)) /* sync error           */
        return FALSE;                       /* last sequence        */
    
    if (seq->offset)
        seq->offset -= (strlen(buffer) + 1);

    seq->name    = LXStrcpy(seq->name,    sGetName(buffer));
    seq->comment = LXStrcpy(seq->comment, sGetComment(buffer));

    /* --------------------------
     * read sequence part
     */

    while ((buffer = sReadLine(streamin, READ_NEXT, INDEXED)) != NULL) {

        if (*buffer == LX_FASTQ_QUAL_BEGIN)
            break;

        if (*buffer == LX_FASTQ_SEQ_BEGIN)
            break;

        long readlen = strlen(buffer);
    
        buflen += readlen;
        
        if (! sAssertSeqBuffer(seq, buflen))
          return FALSE;

        (void) memcpy(seq->seq + seq->length, buffer, readlen);
    
        seq->length = buflen;
    }

    seq->seq[seq->length] = '\000';

    if ((! buffer) || (*buffer != LX_FASTQ_QUAL_BEGIN))
      return FALSE;

    /* --------------------------
     * read quality part
     */

    long quallen = 0;

    while ((buffer = sReadLine(streamin, READ_NEXT, SERIAL)) != NULL) {

        if ((*buffer == LX_FASTQ_SEQ_BEGIN) && (quallen >= buflen))
            break;

        long readlen = strlen(buffer);
    
        quallen += readlen;
        
        if (quallen <= buflen)
            (void) memcpy(seq->qual + quallen - readlen, buffer, readlen);
    }

    seq->qual[seq->length] = '\000';    

    /* --------------------------
     * decode quality
     */

    sDecodeQuality(seq, code);
    
    return (seq->ok = TRUE);
}

/* -------------------------------------------- */
/* write sequence in Fastq format               */
/* -------------------------------------------- */

void LXFastqWriteSequence (FILE *streamou, LXFastqSequencePtr seq)
{
    
    fputc(LX_FASTQ_SEQ_BEGIN, streamou);
    fputs((*(seq->name) ? seq->name : "NO_NAME"), streamou);
    if (*(seq->comment)) {
      fputc(' ', streamou);
      fputs(seq->comment, streamou);
    }
    fputc(LINE_FEED, streamou);

    fputs(seq->seq, streamou);
    fputc(LINE_FEED, streamou);

    fputc(LX_FASTQ_QUAL_BEGIN, streamou);
    fputc(LINE_FEED, streamou);

    sEncodeQuality(seq);
    fputs((char *) seq->qual, streamou);
    sDecodeQuality(seq, LX_FASTQ_SANGER);
    fputc(LINE_FEED, streamou);
}

