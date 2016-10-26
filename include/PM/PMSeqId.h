/* -----------------------------------------------------------------------
 * $Id: PMSeqId.h 1780 2013-02-18 00:50:17Z viari $
 * -----------------------------------------------------------------------
 * @file: PMSeqId.h
 * @desc: sequence header definitions
 *
 * @history:
 * @+ <Gloup> : Jul 06 : first version
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * @docend:
 */

#ifndef _H_PMSeqId

#define _H_PMSeqId

#include "LX/LXFasta.h"

#include "PMSys.h"

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @doc: maximum length of sequence identifier name
 */

#define PM_SEQID_NAME_LEN  256
#define PM_SEQID_COM_LEN   512

/*
 * @doc: characters for direct/reverse strands
 */
 
#define PM_SEQID_DIRECT  'D'
#define PM_SEQID_REVERSE 'R'

/*
 * @doc: parsing error codes
 */
 
#define PM_SEQID_PARSE_ERROR   1
#define PM_SEQID_STRAND_ERROR  2
#define PM_SEQID_FRAME_ERROR   3
#define PM_SEQID_LENGTH_ERROR  4

/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/*
 * @doc: sequence identifier (where hit are mapped)
 * 
 * - name       : sequence name
 * - comment    : sequence comment
 * - strand     : sequence strand ('D', 'R')
 * - frame      : sequence translation frame (1, 2, 3)
 * - length     : nucleotidic sequence length
 */

typedef struct {
  int  strand;
  int  frame;
  long length;
  char name[PM_SEQID_NAME_LEN + 2];
  char comment[PM_SEQID_COM_LEN + 2];
} PMSeqId, *PMSeqIdPtr;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc:
 * parse SeqId from fasta Header
 * usually used to retrieve nucleic sequence information
 * from proteic fasta sequence header.
 * @param fastaSeq LXFastaSequencePtr fasta sequence
 *        fasta name has the form : <name>@(R|D)[1|2|3]
 *        fasta comment starts with nucleic sequence length
 * @param seqId PMSeqId* to fill with information
 * @return error code (LX_NO_ERROR if no error)
 */

int PMFasta2SeqId(LXFastaSequence *fastaSeq, PMSeqId *seqId);

/*
 * @doc:
 * parse SeqId from fasta Header
 * same as previous but will never fail (i.e. make default
 * seqId in case of parsing error)
 * @param fastaSeq LXFastaSequencePtr fasta sequence
 *        fasta name has the form : <name>@(R|D)[1|2|3]
 *        fasta comment starts with nucleic sequence length
 * @param seqId PMSeqId* to fill with information
 * @return LX_NO_ERROR
 */

int PMFasta2SeqIdSafe(LXFastaSequence *fastaSeq, PMSeqId *seqId);

/*
 * @doc:
 * make a valid Fasta header from seqId
 * usually used to make a well formed proteic fasta header
 * from nucleic sequence information
 * @param seqId PMSeqId* to make Fasta header from
 * @param fastaSeq LXFastaSequencePtr fasta sequence to set the header
 * @return error code (LX_NO_ERROR if no error)
 */

int PMSeqId2Fasta(PMSeqId *seqId, LXFastaSequence *fastaSeq);


/*
 * @doc:
 * Read a seqId from buffer 
 * @param buffer char* to read in
 * @param seqId PMSeqId* seqId to read
 * @return error code depending on read issue
 * LX_NO_ERROR, LX_IO_ERROR
 */

int PMParseSeqId(char *buffer, PMSeqId *seqId);

/*
 * @doc:
 * Read a seqId from file 
 * @param streamin FILE* to read in
 * @param seqId PMSeqId* seqId to read
 * @return error code depending on read issue
 * LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */

int PMReadSeqId(FILE *streamin, PMSeqId *seqId);

/*
 * @doc:
 * Write seqId into file
 * @param streamou FILE* to write in
 * @param seqId PMSeqId* seqId to write
 * @return error code depending on write issue
 * LX_NO_ERROR, LX_IO_ERROR
 */

int PMWriteSeqId(FILE *streamou, PMSeqId *seqId);

/*
 * @doc:
 * [debug] print SeqId information
 * @param streamou FILE* to write into
 * @param seqId PMSeqId* to print
 */

void PMDebugSeqId(FILE *streamou, PMSeqId *seqId);

#endif
