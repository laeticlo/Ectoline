/** -----------------------------------------------------------------------
 * @file: LXFasta.h
 * @desc: Fasta format I/O
 * 
 * @history:
 * @+ <Gloup> : Aug 92 : first version
 * @+ <Gloup> : Nov 95 : revision
 * @+ <Gloup> : Nov 00 : adapted to Mac_OS_X
 * @+ <Gloup> : Jan 01 : adapted to LX_Ware
 * @+ <Wanou> : Dec 01 : last revision
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * @+ <Gloup> : Jan 13:  reduced memory for name and comment
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LXfasta library<br>
 * Fasta sequences input/output
 *
 * Revision 1.2  2002/07/11 09:09:40  viari
 * Memory improvement : reduced memory allocation.
 *
 * @docend:
 */

#ifndef _H_LXFasta

#define _H_LXFasta

#include "LXSys.h"

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */


/*
 * @doc:  minimum buffer size for sequence
 */

#define LX_FASTA_MIN_BUFSIZ 32

/*
 * @doc:  default # of chars per line for fasta sequence output
 */

#define LX_FASTA_CHARS_PER_LINE 50

/*
 * @doc:  sequence introducer (Fasta format)
 */

#define LX_FASTA_SEQ_BEGIN  '>'

/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/**
 * @doc: Fasta sequence structure
 * - ok      : Bool  - status flag
 * - length  : long  - sequence length
 * - offset  : long - sequence file offset (for indexed operations) 
 * - name    : char* - sequence name
 * - comment : char* - sequence comment
 * - seq     : char* - sequence buffer
 */

typedef struct {                             /* -- FastaSequence --     */
        Bool    ok;                          /* status flag             */
        long    length,                      /* sequence length         */
                offset;                      /* file offset             */
        char    *name,                       /* sequence name           */
                *comment,                    /* sequence comment        */
                *seq;                        /* sequence buffer         */
} LXFastaSequence, *LXFastaSequencePtr;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * Allocate a new empty FastaSequence structure
 * @param bufferSize long optional initial bufferSize
 *        if (bufferSize <= 0) then use a default size
 * @return LXFastaSequencePtr
 */

LXFastaSequencePtr LXFastaNewSequence ( long bufferSize );

/*
 * @doc: 
 * Free a previously allocated FastaSequence structure
 * @return NULL
 */

LXFastaSequencePtr LXFastaFreeSequence  ( LXFastaSequencePtr seq );

/*
 * @doc:
 * Read next fasta entry (serial reading)
 * @param streamin FILE* to read in
 * @param seq LXFastaSequencePtr sequence to read
 * @return TRUE if more sequence to read
 * @sample
 * 
 * <b>Example:</b>
 * <pre><code>
 * LXFastaSequencePtr seq = LXFastaNewSequence();
 * while (LXFastaReadSequence(streamin, seq)) {
 *   if (seq->ok)
 *      MyProcessSequence(seq);
 * }
 * </code></pre>
 */

Bool LXFastaReadSequence  (FILE *streamin, LXFastaSequencePtr seq);

/*
 * @doc:
 * Read next fasta entry (// reading)
 * @param streamin FILE* to read in
 * @param seq LXFastaSequencePtr sequence to read
 * @return TRUE if more sequence to read
 */

Bool LXFastaGetSequence (FILE *streamin, LXFastaSequencePtr seq);

/*
 * @doc:
 * Check fasta sequence integrity
 * @param seq LXFastaSequencePtr sequence to check
 * @param upper Bool additionnaly check that sequence is uppercase
 * @return TRUE if sequence is ok
 */

Bool LXFastaCheckSequence (LXFastaSequencePtr seq, Bool upper);

/**
 * @doc:
 * Write fasta format sequence
 * @param streamou FILE* to write in
 * @param seq LXFastaSequencePtr sequence to write
 * @param char_per_line int maximum number of characters
 *        per line
 */
void LXFastaWriteSequence (FILE *streamou, LXFastaSequencePtr seq,
                           int char_per_line);

#endif







