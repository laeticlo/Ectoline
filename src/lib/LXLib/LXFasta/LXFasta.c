/* -----------------------------------------------------------------------
 * $Id: LXFasta.c 1782 2013-02-23 00:11:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXFasta.c
 * @desc: sequence IO in fasta format
 * 
 * @history:
 * @+ <Gloup> : Aug 92 : first version
 * @+ <Gloup> : Nov 95 : last revised version
 * @+ <Wanou> : Dec 01 : adapted to LX_Ware 
 * @+ <Gloup> : Feb 02 : corrected comment bug 
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * @+ <Gloup> : Jan 13 : read buffer moved to LXLineBuf library
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LXfasta C program<br>
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXString.h"
#include "LX/LXFasta.h"

#define PUSH_BACK TRUE
#define READ_NEXT FALSE

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

static Bool sAssertSeqBuffer(LXFastaSequencePtr seq, size_t size) {
    if (size >= LXAllocSize(seq->seq)) {
        size = 2 * size + 1;
        char *tbuf = REALLOC(seq->seq, char, size);
        if (! tbuf) return FALSE;
        seq->seq = tbuf;
    }
    return TRUE;
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
/* @static: returns sequence name (FASTA)       */
/* -------------------------------------------- */

static char *sGetName(char *buffer)
{
    char save;

    static char sName[BUFSIZ+1];

    save = buffer[BUFSIZ];
    buffer[BUFSIZ] = '\000';
    
    if (sscanf(buffer + 1, "%s", sName) != 1)
        (void) strcpy(sName, "NO_NAME");

    buffer[BUFSIZ] = save;

    return sName;
}

/* -------------------------------------------- */
/* @static: returns sequence comment (FASTA)    */
/* -------------------------------------------- */
static char *sGetComment(char *buffer)
{
    char   save, *space;

    static char sComment[BUFSIZ+1];

    save = buffer[BUFSIZ];
    buffer[BUFSIZ] = '\000';
    
    space = sNextSpace(buffer);
    
    strcpy(sComment, (space ? space + 1 : "<no comment>"));

    buffer[BUFSIZ] = save;

    return sComment;
}

/* -------------------------------------------- */
/* free Fasta sequence                          */
/* -------------------------------------------- */
LXFastaSequencePtr LXFastaFreeSequence(LXFastaSequencePtr seq)
{
    if (seq) {
        IFFREE(seq->name);
        IFFREE(seq->comment);
        IFFREE(seq->seq);
        FREE(seq);
    }

    return NULL;
}
    
/* -------------------------------------------- */
/* create new fasta sequence                    */
/* -------------------------------------------- */
LXFastaSequencePtr LXFastaNewSequence(long bufsize)
{
    LXFastaSequencePtr seq;
    
    if (! (seq = NEW(LXFastaSequence)))
        return NULL;
       
    seq->length   =  0;
    seq->offset   = -1;
    seq->name     = NULL;
    seq->comment  = NULL;

    bufsize = (bufsize <= 0 ? LX_FASTA_MIN_BUFSIZ : bufsize);
    
    if (! (seq->seq = LXNewStr(bufsize)))
        return LXFastaFreeSequence(seq);

    seq->ok = TRUE;
    
    return seq;
}

/* -------------------------------------------- */
/* Serial read Fasta sequence                   */
/* returns : FALSE -> last sequence             */
/*           TRUE  -> more to read              */
/*           <but> you must check seq->ok !     */
/*           that may be FALSE (memory error)   */
/* -------------------------------------------- */
Bool LXFastaReadSequence(FILE *streamin, LXFastaSequencePtr seq)
{
    size_t  buflen;
    char   *buffer;

    seq->ok = FALSE;                        /* assume error         */

    buflen = seq->length = 0; 
    
    seq->offset = ftell(streamin);

    buffer = sReadLine(streamin, READ_NEXT, SERIAL);

    if (   (buffer == NULL) 
        || (*buffer != LX_FASTA_SEQ_BEGIN)) /* sync error           */
        return FALSE;                       /* last sequence        */
    
    if (seq->offset)
        seq->offset -= (strlen(buffer) + 1);

    seq->name    = LXStrcpy(seq->name,    sGetName(buffer));
    seq->comment = LXStrcpy(seq->comment, sGetComment(buffer));

    while ((buffer = sReadLine(streamin, READ_NEXT, SERIAL)) != NULL) {
    
        if (*buffer == LX_FASTA_SEQ_BEGIN) {
            (void) sReadLine(streamin, PUSH_BACK, SERIAL);
            break;
        }

        size_t readlen = strlen(buffer);
    
        buflen += readlen;
        
        if (! sAssertSeqBuffer(seq, buflen))
          return TRUE;   /* but seq->ok is FALSE  */
          
        (void) memcpy(seq->seq + seq->length, buffer, readlen);
        
        seq->length = buflen;
    }

    seq->seq[seq->length] = '\000';

    return (seq->ok = TRUE);
}

/* -------------------------------------------- */
/* Indexed read Fasta sequence                  */
/* returns : FALSE -> memory error              */
/*           TRUE  -> read ok                   */
/* -------------------------------------------- */
Bool LXFastaGetSequence(FILE *streamin, LXFastaSequencePtr seq)
{
    size_t  buflen;
    char   *buffer;

    seq->ok = FALSE;                        /* assume error         */

    buflen = seq->length = 0; 
    
    fseek(streamin, seq->offset, SEEK_SET);

    buffer = sReadLine(streamin, READ_NEXT, INDEXED);

    if (   (buffer == NULL) 
        || (*buffer != LX_FASTA_SEQ_BEGIN)) /* sync error           */
        return FALSE;                       /* last sequence        */
    
    if (seq->offset)
        seq->offset -= (strlen(buffer) + 1);

    seq->name    = LXStrcpy(seq->name,    sGetName(buffer));
    seq->comment = LXStrcpy(seq->comment, sGetComment(buffer));

    while ((buffer = sReadLine(streamin, READ_NEXT, INDEXED)) != NULL) {

        if (*buffer == LX_FASTA_SEQ_BEGIN)
            break;

        size_t readlen = strlen(buffer);
    
        buflen += readlen;

        if (! sAssertSeqBuffer(seq, buflen))
          return FALSE;
        
        (void) memcpy(seq->seq + seq->length, buffer, readlen);
    
        seq->length = buflen;
    }

    seq->seq[seq->length] = '\000';

    return (seq->ok = TRUE);
}

/* -------------------------------------------- */
/* Check Fasta sequence                         */
/* -------------------------------------------- */

Bool LXFastaCheckSequence (LXFastaSequencePtr seq, Bool upper)
{
    long   i, n;
    char   *s;

    if (! seq->ok)
       return FALSE;

    n = seq->length;
    s = seq->seq;

    if (upper) {
        for (i = 0 ; i < n ; i++) {
            if (! isupper(*s++))
                return (seq->ok = FALSE);
        }
    }
    else {
        for (i = 0 ; i < n ; i++) {
            if (! isalpha(*s++))
                return (seq->ok = FALSE);
        }
    }

    return TRUE;
}

/* -------------------------------------------- */
/* write sequence in Fasta format               */
/* -------------------------------------------- */

void LXFastaWriteSequence(FILE *streamou, LXFastaSequencePtr seq, int char_per_line)
{
    long i, nlines, rest;
    char *buf, *end, tempo;

    fputc(LX_FASTA_SEQ_BEGIN, streamou);
    fputs((*(seq->name)    ? seq->name    : "NO_NAME")   , streamou);
    fputc(' ', streamou);
    fputs((*(seq->comment) ? seq->comment : "<no comment>"), streamou);
    fputc(LINE_FEED, streamou);

    nlines = seq->length / char_per_line;

    buf = seq->seq;

    for (i = 0 ; i < nlines ; i++) {
        end = buf + char_per_line;
        tempo = *end;
        *end = '\000';
        fputs(buf, streamou);
        fputc(LINE_FEED , streamou);
        *end = tempo;
        buf += char_per_line;
    }

    if ((rest = (seq->length % char_per_line)) != 0) {
       end = buf + rest;
       tempo = *end;
       *end = '\000';
       fputs(buf, streamou);
       fputc(LINE_FEED , streamou);
       *end = tempo;
    }
}
