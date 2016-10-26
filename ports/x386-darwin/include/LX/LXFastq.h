/** -----------------------------------------------------------------------
 * @file: LXFastq.h
 * @desc: Fastq format I/O
 * 
 * @history:
 * @+ <Gloup> : Aug 92 : first version
 * @+ <Gloup> : Nov 95 : revision
 * @+ <Gloup> : Nov 00 : adapted to Mac_OS_X
 * @+ <Gloup> : Jan 01 : adapted to LX_Ware
 * @+ <Wanou> : Dec 01 : last revision
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * @+ <Gloup> : Jan 13 : added nonSanger formats
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LXfastq library<br>
 * Fastq sequences input/output
 *
 * Revision 1.2  2002/07/11 09:09:40  viari
 * Memory improvement : reduced memory allocation.
 *
 * @docend:
 */

#ifndef _H_LXFastq

#define _H_LXFastq

#ifndef _H_LXSys
#include "LXSys.h"
#endif

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @doc:  minimum buffer size for sequence
 */

#define LX_FASTQ_MIN_BUFSIZ 16

/*
 * @doc:  sequence introducer (Fastq format)
 */

#define LX_FASTQ_SEQ_BEGIN   '@'
#define LX_FASTQ_QUAL_BEGIN  '+'

/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/**
 * @doc: Fastq sequence structure
 * - ok      : Bool  - status flag
 * - length  : long  - sequence length
 * - offset  : long - sequence file offset (for indexed operations) 
 * - bufsize : long - [internal] size of current sequence buffer
 * - name    : *char - sequence name
 * - comment : *char - sequence comment
 * - seq     : char* - sequence buffer
 * - qual    : unsigned char* - quality 
 */

typedef struct {                             /* -- FastqSequence --     */
        Bool    ok;                          /* status flag             */
        long    length,                      /* sequence length         */
                offset;                      /* file offset             */
        char    *name,                       /* sequence name           */
                *comment,                    /* sequence comment        */
                *seq;                        /* sequence buffer         */
        unsigned char *qual;                 /* qualities               */
} LXFastqSequence, *LXFastqSequencePtr;

/**
 * @doc: available Fastq quality formats
 *  LX_FASTQ_SANGER   : standard (Phred+33) encoding (illumina >= 1.8) [default]
 *  LX_FASTQ_ILLUMINA : illumina 1.3 to 1.7 (Phred+64) encoding
 *  LX_FASTQ_SOLEXA   : old solexa and illumina < 1.3 (Phred+64/Log(p/1-p) encoding
 */

typedef enum {
  LX_FASTQ_SANGER = 0, /* standard (Phred+33) encoding (illumina >= 1.8) */
  LX_FASTQ_ILLUMINA,   /* illumina 1.3 to 1.7 (Phred+64) encoding        */
  LX_FASTQ_SOLEXA      /* old solexa and illumina < 1.3  */
} LXFastqEncoding;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * Allocate a new empty FastqSequence structure
 * @param bufferSize long optional initial bufferSize
 *        if (bufferSize <= 0) then use a default size
 * @return LXFastqSequencePtr
 */

LXFastqSequencePtr LXFastqNewSequence ( long bufferSize );

/*
 * @doc: 
 * Free a previously allocated FastqSequence structure
 * @return LXFastqSequencePtr
 */

LXFastqSequencePtr LXFastqFreeSequence  ( LXFastqSequencePtr seq );

/*
 * @doc:
 * Read next fastq entry (serial reading)
 * @param streamin FILE* to read in
 * @param seq LXFastqSequencePtr sequence to read
 * @return TRUE if more sequence to read
 * @sample
 * 
 * <b>Example:</b>
 * <pre><code>
 * LXFastqSequencePtr seq = LXFastqNewSequence();
 * while (LXFastqReadSequence(streamin, seq)) {
 *   if (seq->ok)
 *      MyProcessSequence(seq);
 * }
 * </code></pre>
 */

Bool LXFastqReadSequence(FILE *streamin, LXFastqSequencePtr seq, LXFastqEncoding code);

/*
 * @doc:
 * Read next fastq entry (// reading)
 * @param streamin FILE* to read in
 * @param seq LXFastqSequencePtr sequence to read
 * @return TRUE if more sequence to read
 */

Bool LXFastqGetSequence (FILE *streamin, LXFastqSequencePtr seq, LXFastqEncoding code);

/**
 * @doc:
 * Write fastq format sequence
 * @param streamou FILE* to write in
 * @param seq LXFastqSequencePtr sequence to write
 */
void LXFastqWriteSequence (FILE *streamou, LXFastqSequencePtr seq);

#endif
