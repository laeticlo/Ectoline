/* -----------------------------------------------------------------------
 * $Id: PMSeqId.c 1810 2013-02-25 10:04:43Z viari $
 * -----------------------------------------------------------------------
 * @file: PMSeqId.c
 * @desc: PepMap SeqId library
 * 
 * @history:
 * @+ <Gloup> : Jul 06 : first version 
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * PM SeqId library<br>
 *
 * @docend:
 */

#include "LX/LXString.h"

#include "PM/PMSeqId.h"

#define MAX_BUF_LEN PM_SEQID_NAME_LEN + PM_SEQID_COM_LEN

/* ----------------------------------------------------- */
/* get valid comment                                     */
/* ----------------------------------------------------- */

static const char *sValidComment(const char *comment) {
  static const char *sNoComment = "<no comment>";
  if (comment && *comment)
    return comment;
  return sNoComment;
}

/* ----------------------------------------------------- */
/* @static: read seqId from buffer                       */
/* ----------------------------------------------------- */

static int sReadFormatSeqId(char *buffer, PMSeqId *seqId)
{
  char strand[2];
  char comment[PM_SEQID_COM_LEN + 2];
  
  if (sscanf(buffer, "%s%1s%d%ld%[ -~]", seqId->name, strand, &(seqId->frame),
                                         &(seqId->length), comment) != 5) {
    fprintf(stderr, "// IO-Format error at: %s\n", buffer);
    return IO_ERROR();
  }

  seqId->strand = strand[0];
  (void) strcpy(seqId->comment, sValidComment(comment));

  return LX_NO_ERROR;
}

/* ----------------------------------------------------- */
/* @static: write seqId                                  */
/* ----------------------------------------------------- */

static int sWriteFormatSeqId(FILE *streamou, PMSeqId *seqId)
{
  int nwrite;

  nwrite= fprintf(streamou, "%s %c %d %ld %s\n",
                  	        seqId->name,
                            seqId->strand,
                            seqId->frame,
                            seqId->length,
                            sValidComment(seqId->comment));

  return ((nwrite <= 0) ? IO_ERROR() : LX_NO_ERROR);
} 

/* -------------------------------------------- */
/* Parse funcs                                  */
/* -------------------------------------------- */

/* ---------------------------------------------------- */
/* parse SeqId from Fasta header                        */
/* usually to retrieve nucleic sequence information     */
/* from proteic fasta sequence header.                  */
/*  fasta name has the form : name@(D|R)[1|2|3]@length  */
/* ---------------------------------------------------- */

int PMFasta2SeqId(LXFastaSequence *fastaSeq, PMSeqId *seqId)
{
  char strand;
  
  // [AV] make default for unformatted fasta name
  (void) strncpy(seqId->name, fastaSeq->name, PM_SEQID_NAME_LEN);
  (void) strncpy(seqId->comment, fastaSeq->comment, PM_SEQID_COM_LEN);
  seqId->length = fastaSeq->length;
  seqId->frame = 0;
  seqId->strand = 'X';
  
  if (sscanf(fastaSeq->name, "%[^@]@%c%1d@%ld",
                             seqId->name, &strand,
                             &(seqId->frame), &(seqId->length)) != 4)
    return PM_SEQID_PARSE_ERROR;

  seqId->strand = strand;
  
  if (   (seqId->strand != PM_SEQID_DIRECT) 
      && (seqId->strand != PM_SEQID_REVERSE))
    return PM_SEQID_STRAND_ERROR;
  
  if ((seqId->frame < 1) || (seqId->frame > 3))
    return PM_SEQID_FRAME_ERROR;
    
  if (seqId->length <= 0)
    return PM_SEQID_LENGTH_ERROR;

  (void) strncpy(seqId->comment, fastaSeq->comment, PM_SEQID_COM_LEN);
  
  return LX_NO_ERROR;
}

/* ---------------------------------------------------- */
/* parse SeqId from Fasta header                        */
/* same as above but with default behavior in case of   */
/* failure                                              */
/* ---------------------------------------------------- */

int PMFasta2SeqIdSafe(LXFastaSequence *fastaSeq, PMSeqId *seqId)
{
  if (PMFasta2SeqId(fastaSeq, seqId) == LX_NO_ERROR)
    return LX_NO_ERROR;

  (void) strncpy (seqId->name, fastaSeq->name, PM_SEQID_NAME_LEN);
    
  seqId->strand = PM_SEQID_DIRECT;
  seqId->frame  = 1;
  seqId->length = fastaSeq->length;

  return  LX_NO_ERROR; 
}

/* ---------------------------------------------------- */
/* make a full Fasta header from seqId                  */
/* usually to make a well formed proteic fasta header   */
/* from nucleic sequence information                    */
/* ---------------------------------------------------- */

int PMSeqId2Fasta(PMSeqId *seqId, LXFastaSequence *fastaSeq)
{
  char buffer[MAX_BUF_LEN + 2];
  
  if (sprintf(buffer, "%s@%c%1d@%ld", 
                      seqId->name, seqId->strand,
                      seqId->frame, seqId->length) <= 0)
    return PM_SEQID_PARSE_ERROR;
  
  fastaSeq->name = LXStrcpy(fastaSeq->name, buffer);
  fastaSeq->comment = LXStrcpy(fastaSeq->comment,
                               sValidComment(seqId->comment));

  return LX_NO_ERROR;
}

/* ---------------------------------------------------- */
/* read seqId from buffer                               */
/* ---------------------------------------------------- */

int PMParseSeqId(char *buffer, PMSeqId *seqId)
{
  return sReadFormatSeqId(buffer, seqId);
}

/* ---------------------------------------------------- */
/* read seqId from file                                 */
/* ---------------------------------------------------- */

int PMReadSeqId(FILE *streamin, PMSeqId *seqId)
{
  char buffer[BUFSIZ];

   /* get next line in stream */

  if (! LXGetLine(buffer, sizeof(buffer), streamin))
      return LX_EOF;

  /* parse line */

  return sReadFormatSeqId(buffer, seqId);
}

/* ---------------------------------------------------- */
/* write seqId to file                            	    */
/* ---------------------------------------------------- */

int PMWriteSeqId(FILE *streamou, PMSeqId *seqId)
{
  return sWriteFormatSeqId(streamou, seqId);
}

/* -------------------------------------------- */
/* debug funcs                                  */
/* -------------------------------------------- */

/* ---------------------------------------------------- */
/* print seqId debug information                        */
/* ---------------------------------------------------- */

void PMDebugSeqId(FILE *streamou, PMSeqId *seqId)
{
    fprintf(streamou, "// PMSeqId\n");
    fprintf(streamou, "// name      : %s\n",  seqId->name);
    fprintf(streamou, "// strand    : %c\n",  seqId->strand);
    fprintf(streamou, "// frame     : %d\n",  seqId->frame);
    fprintf(streamou, "// length    : %ld\n", seqId->length);
    fprintf(streamou, "//\n");
}
