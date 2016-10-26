/* -----------------------------------------------------------------------
 * $Id: PMTrans.c 1810 2013-02-25 10:04:43Z viari $
 * -----------------------------------------------------------------------
 * @file: PMtrans.c
 * @desc: translate a fasta nucleic sequence in the 6 frames
 *
 * @history:
 * @+ <Gloup> : Jan 96 : PWG version                
 * @+ <Wanou> : Jan 02 : revision & adaptation to PepMap 
 * @+ <Wanou> : Mar 02 : last revision to being included in PepMap release v1.0
 * @+ <Gloup> : Jul 06 : code revision          
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * Chromosome translation program <br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX.h"
#include "PM.h"

/* ----------------------------------------------- */
static void sStartupBanner(int argc, char *argv[])
{
  int i;
  fprintf(stderr,"# ------------------------------\n");
  fprintf(stderr,"# PepMap PMTrans v %s\n", VERSION);
  fprintf(stderr,"#\n");
  fprintf(stderr,"# Translate Fasta DNA sequence(s) into\n");
  fprintf(stderr,"# fastA aminoacid sequences (6 frames)\n");
  fprintf(stderr,"# ------------------------------\n");
  fprintf(stderr,"# run: ");
  for (i = 0 ; i < argc ; i++)
    fprintf(stderr,"%s ", argv[i]);
  fprintf(stderr,"\n");
  fprintf(stderr,"# ------------------------------\n");
}

/* ----------------------------------------------- */
static void sUsage(int err)
{
  fprintf(stderr,"Usage: PMTrans [options]\n");
  fprintf(stderr,"type PMTrans -h for help\n");
  exit(err);
}

/* ----------------------------------------------- */
static void sHelp()
{
  int i;
  fprintf(stderr,"Usage: PMTrans [options]\n");
  fprintf(stderr," options:\n");
  fprintf(stderr,"   -c code        : genetic code to use\n");
  fprintf(stderr,"             code : species\n");
  for (i = 0 ; i < LX_GENETIC_MAX_CODES ; i++) {
    fprintf(stderr,"                %d : %s\n", i, gLXGeneticCode[i].title);
  }
  fprintf(stderr,"                    (default = 0)\n");
  fprintf(stderr," -h               : print this help\n");
  fprintf(stderr," -i fastaFile     : input chromosome(s) filename (fasta format)\n");
  fprintf(stderr,"                    (default = <stdin>)\n");
  fprintf(stderr,"                    note: may contain more than one sequence\n");
  fprintf(stderr," -o outputFile    : output translated chromosome(s) filename\n");
  fprintf(stderr,"                    (default = <stdout>)\n");
  fprintf(stderr," -v               : verbose mode (add some internal information)\n");
  fprintf(stderr,"                    (default = off)\n");
  exit(0);
}

/* ----------------------------------------------- */

main(int argc, char *argv[])
{
  long nbseq;
  int  istrand, iframe;
  int  opt, code;
  
  int strands[] = {PM_SEQID_DIRECT, PM_SEQID_REVERSE};
  
  Bool verbose;
  
  LXFastaSequence *nucSeq, *prtSeq;
  
  PMSeqId seqId;
  
  char inputFileName[BUFSIZ];
  char outputFileName[BUFSIZ];
  char buffer[BUFSIZ];
  
  extern char *optarg;  /* externs for getopts (3C) */

  /* -------------------------- */
  /* setup default parameters   */
  
  code            = 0;
  verbose         = FALSE;
  *inputFileName  ='\000';
  *outputFileName ='\000';

  /* -------------------------- */
  /* get user's parameters      */

  while ((opt = getopt(argc, argv, "c:hHi:o:v")) != -1) {
  
    switch (opt) {
    
      case 'c':                                  // genetic code
        if (    (sscanf(optarg, "%d", &code) != 1) 
             || (code < 0) || (code > LX_GENETIC_MAX_CODES)) {
          fprintf(stderr,"invalid code value: -c (0-%d)\n", LX_GENETIC_MAX_CODES);
          sUsage(LX_ARG_ERROR);
        }
        break;
        
      case 'H' :                                  // help
      case 'h' :
        sHelp();
        break;
        
      case 'i' :                                  // input file
        (void) strcpy(inputFileName, optarg);
        if(! LXAssignToStdin(inputFileName)) {
          fprintf(stderr,"Cannot open : %s\n", inputFileName);
          exit (LX_IO_ERROR); 
        }
        break;

      case 'o' :                                  // output (tag) filename
        (void) strcpy(outputFileName, optarg);
        if (! LXAssignToStdout(outputFileName)) {
          fprintf(stderr,"Cannot create : %s\n", outputFileName);
          exit (LX_IO_ERROR);
        }
        break;

      case 'v' :                                  // verbose mode
        verbose = TRUE;
        break;

      case '?' :                                  // oops !
        sUsage(LX_ARG_ERROR);
        break;
      }
  }

  /* -------------------------- */
  /* start processing           */
  
  sStartupBanner(argc, argv);

  (void) LXStrCpuTime(TRUE);

  if (! (nucSeq = LXFastaNewSequence(0)))
    MEMORY_ERROR();

  /* ---------------------- */
  /* main fasta loop        */
  
  nbseq = 0;
  
  while (LXFastaReadSequence(stdin, nucSeq)) {

    nbseq++;
    
    if (! nucSeq->ok) {
      sprintf(buffer,"# error with sequence No %ld (skipped)\n", nbseq);
      LX_WARNING(buffer);
      continue;
    }

    if (verbose)
      fprintf(stderr, "# read sequence %s (%ld nt)\n",
            nucSeq->name, nucSeq->length);

    if (! (prtSeq = LXFastaNewSequence(nucSeq->length + 1)))
      MEMORY_ERROR();

    (void) strcpy(seqId.name, nucSeq->name);
    (void) strcpy(seqId.comment, nucSeq->comment);

    seqId.length = nucSeq->length;
    
    for (istrand = 0 ; istrand <= 1 ; istrand++) {

      seqId.strand = strands[istrand];
      
      for (iframe = 0 ; iframe < 3 ; iframe++) {
  
        (void) strcpy(prtSeq->seq, nucSeq->seq + iframe);
        
        LXBioSeqTranslate(prtSeq->seq, code);
  
        prtSeq->length = (nucSeq->length - iframe) / 3;
	
        seqId.frame = iframe + 1;

#if 1
// [AV] version originale
       if (PMSeqId2Fasta(&seqId, prtSeq) != LX_NO_ERROR)
         LX_ERROR("write format error", 10);
#else
// [AV] version JET
// [AV] FIXME : le commentaire est perdu
        sprintf(prtSeq->name, "%s@%c%1d@%d", 
                      seqId.name, seqId.strand,
                      seqId.frame, seqId.length);
#endif

        if (verbose)
          fprintf(stderr, "# write sequence %s (%ld aa)\n",
                         prtSeq->name, prtSeq->length);
          
        LXFastaWriteSequence(stdout, prtSeq, LX_FASTA_CHARS_PER_LINE);

      }

      if (istrand == 0)
        (void) LXBioSeqReverseComplement(nucSeq->seq);
    }

    (void) LXFastaFreeSequence(prtSeq);
    
  }

  /* ------------------- */
  /* end of program      */

  (void) LXFastaFreeSequence(nucSeq);
 
  exit(0);
}



    


