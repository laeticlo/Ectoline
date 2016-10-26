/* test Sequence Id */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXFasta.h"
#include "PM.h"

main() {

  int i;
  LXFastaSequencePtr nseq, pseq[6];
  PMSeqId seqId;

  nseq = LXFastaNewSequence(0);

  (void) LXFastaReadSequence(stdin, nseq);

  (void) printf("%s\n", nseq->name);
  (void) printf("%s\n", nseq->comment);
  
  for (i = 0 ; i < 6 ; i++) {
    pseq[i] = LXFastaNewSequence(0);
  }

  /* fastaHeader from SeqId */
  
  (void) strcpy(seqId.name, nseq->name);
  (void) strcpy(seqId.comment, nseq->comment);
  seqId.length = nseq->length;
  
  seqId.strand = PM_SEQID_DIRECT;
  
  for (i = 0 ; i < 3 ; i++) {
    seqId.frame = i + 1;
    (void) PMSeqId2Fasta(&seqId, pseq[i]);
    (void) printf("%s\n", pseq[i]->name);
    (void) printf("%s\n", pseq[i]->comment);
  }
  
  seqId.strand = PM_SEQID_REVERSE;
  
  for (i = 0 ; i < 3 ; i++) {
    seqId.frame = i + 1;
    (void) PMSeqId2Fasta(&seqId, pseq[i+3]);
    (void) printf("%s\n", pseq[i+3]->name);
    (void) printf("%s\n", pseq[i]->comment);
  }

  /* parse SeqId from fasta header */
  
  for (i = 0 ; i < 6 ; i++) {
    (void) PMFasta2SeqId(pseq[i], &seqId);
    PMDebugSeqId(stdout, &seqId);
  }

  LXFastaFreeSequence(nseq);

  for (i = 0 ; i < 6 ; i++) {
    (void) LXFastaFreeSequence(pseq[i]);
  }

  exit(0);
}
