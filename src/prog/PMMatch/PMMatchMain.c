/* -----------------------------------------------------------------------
 * $Id: PMMatchMain.c 1366 2011-08-22 16:40:49Z viari $
 * -----------------------------------------------------------------------
 * @file: PMMatchMain.c
 * @desc: PepMap Chromosome/Prot Matcher main program (Multi-Tags version) 
 *
 * @history:
 * @+ <Wanou> : Jul 02 : first version
 * @+ <Gloup> : Aug 06 : complete code revision
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PepMap PMMatch (MT) main program <br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX.h"
#include "PM.h"

#include "PMMatch.h"
#include "assert.h"

/* -------------------------------------------------------------------- */
/* Local functs                                                         */
/* -------------------------------------------------------------------- */

/* ----------------------------------------------- */
static HitScoreType sGetHitScoreType(char *type)
{
   if (*type == 'P')
     return PSTHitScoreType;

   if (*type == 'R')
     return REMTHitScoreType;

   if (*type == 'D')
     return Dancink99HitScoreType;

   if (*type == 'S')
     return RankSumHitScoreType;

   return UnknownHitScoreType;
}


/* ----------------------------------------------- */
static int sReadSpectrumFile(char *filename, LXStackpPtr *spectrumStack, PMSpectrumReader spectrumReader)
{
  FILE *spectrumFile;
  PMSpectrum *spRaw;

  if (! (spectrumFile = LXOpenFile(filename, "r")))
    return IO_ERROR();
    
  *spectrumStack = LXNewStackp(LX_STKP_MIN_SIZE);
  
  spRaw = PMNewSpectrum();

  while (spectrumReader(spectrumFile, spRaw) == LX_NO_ERROR) {
  
    LXPushpIn(*spectrumStack, spRaw );

    spRaw = PMNewSpectrum();
  }

  PMFreeSpectrum(spRaw);

  fclose(spectrumFile);
  
  return LX_NO_ERROR;
}


/* ----------------------------------------------- */
/* retrieve spectrum associated to tag from tag id */
/* tag id has the form : string.int.int            */
/* where the seconf int indicates the spectrum     */
/* index.                                          */

static int sRetrieveSpectrum(PMTag *tag, LXStackpPtr spectrumStack )
{
  char buffer[BUFSIZ];

  int index;
  
  char *lastDot;

  // line is : string.int.int
  (void) strcpy( buffer, tag->id );
  
  if (! (lastDot = strrchr( buffer, '.')) ) {
     fprintf(stderr, "# Wrong value for PST id : %s (not any dot)\n", tag->id);
     return LX_IO_ERROR;
  }
  
  *lastDot = '\0';  // remove the last dot and integer
  
  if (! (lastDot = strrchr( buffer, '.')) ) {
     fprintf(stderr, "# Wrong value for PST id : %s (no two dots)\n", tag->id);
     return LX_IO_ERROR;
  }
  
  // increment lastDot to keep only the first integer
  
  if (! *(lastDot++) ) {
     fprintf(stderr, "# Wrong value for PST id : %s (empty spectrum index)\n", tag->id);
     return LX_IO_ERROR;
  }
  
  if (   (sscanf( lastDot ,"%d", &index ) != 1)
      || (index <= 0)) {
     fprintf(stderr, "# Wrong value for PST id : %s (spectrum index is not a number)\n", tag->id);
     return LX_IO_ERROR;
  }

  // spectrum count begins at 1: decrease one
  
  tag->spectrum = (PMSpectrum *) LXStackpGetValue( spectrumStack, index - 1 );
  
  if ( ! tag->spectrum) {
     fprintf(stderr, "# PST id : %s and spectrum file are incoherent.\n", tag->id);
     return LX_IO_ERROR;
  }

  return LX_NO_ERROR;
}


/* -------------------------------------------------------------------- */
/* Main Entry point                                                     */
/* -------------------------------------------------------------------- */

main(int argc, char *argv[])
{
  extern char *optarg;
  
  int opt, kuple, nbSeq, nbTags, nbSpectrums;
  
  FILE *fastaFile;

  PMAminoAlphabet alpha;

  PMEnzyme enzyme;
  
  Parameter param;
  
  HashTable  *hashTable;
  
  PMTagStack *tagStack;

  LXStackpPtr spectrumStack;
  
  LXFastaSequence *fastaSeq;
  

  char inputFilename[BUFSIZ],
       outputFilename[BUFSIZ], 
       aminoFilename[BUFSIZ],
       enzymeFilename[BUFSIZ],
       fastaFilename[BUFSIZ],
       spectrumFilename[BUFSIZ],
       spectrumBasename[BUFSIZ],
       buffer[BUFSIZ];
  
  PMSpectrumReader spectrumReader;
 

  /* --------------------------------- */
  /* setup default parameters          */
  
  param.maxMisCleavage     = DFT_MAX_MISCLEAVAGE;
  param.maxOverCleavage    = DFT_MAX_OVERCLEAVAGE;
  param.maxModif           = DFT_MAX_MODIF;
  param.massTolerance      = DFT_TOLERANCE;
  param.metScan            = DFT_MET_SCAN;
  param.differentiateKQ    = FALSE;
  param.partialHit         = FALSE;
  param.verbose            = FALSE;
  param.hitScoreType       = DFT_HITSCORE;
  
  (void) strcpy(aminoFilename,  DFT_AA_FILENAME);
  (void) strcpy(enzymeFilename, DFT_ENZ_FILENAME);
  
  *inputFilename     = '\000';
  *outputFilename    = '\000';
  *fastaFilename     = '\000';
  *spectrumFilename  = '\000';

  spectrumReader = PMReadPKL1Spectrum;
  
  /* --------------------------------- */
  /* get user's parameters             */

  while ((opt = getopt(argc, argv, "a:d:D:e:f:F:Hhi:kn:m:M:o:ps:S:vy:")) != -1) {
  
    switch (opt) {
      
      case 'a' :                                  // Amino-acid filename
        (void) strcpy(aminoFilename, optarg);
        break;
  
      case 'd':                                   // mass tolerance (ppm)
        if (   (sscanf(optarg, "%f", &param.massTolerance) != 1)
            || (param.massTolerance <= 0)) {
          fprintf(stderr, "# Wrong value for mass tolerance\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'D':                                   // mass tolerance (amu)
        if (   (sscanf(optarg, "%f", &param.massTolerance) != 1)
            || (param.massTolerance <= 0)) {
          fprintf(stderr, "# Wrong value for mass tolerance\n");
          ExitUsage(LX_ARG_ERROR);
        }
        param.massTolerance = - param.massTolerance;
        break;

      case 'e' :                                  // Enzyme filename
        (void) strcpy(enzymeFilename, optarg);
        break;

      case 'f' :                                  // Fasta filename
        (void) strcpy(fastaFilename, optarg);
        break;

      case 'F':                                   // input spectrum format
        if ( (sscanf(optarg, "%s", buffer) != 1) ||
             (! (spectrumReader = PMGetSpectrumReader(buffer))) ) {
          fprintf(stderr, "# Unknown data format %s\n", buffer);
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'H' :                                  // help
      case 'h' :
        PrintHelp();
        exit(0);
        break;
  
      case 'i' :                                  // input (tags) filename
        (void) strcpy(inputFilename, optarg);
        if(! LXAssignToStdin(inputFilename)) {
          fprintf(stderr,"# Cannot open : %s\n", inputFilename);
          exit (LX_IO_ERROR); 
        }
        break;

      case 'k' :                                  // differentiate K/Q
        param.differentiateKQ = TRUE;
        break;

      case 'm':                                   // max number of miscleavage
        if (   (sscanf(optarg, "%d", &param.maxMisCleavage) != 1)
            || (param.maxMisCleavage < 0)) {
          fprintf(stderr, "# Wrong value for maximum miscleavage\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'M':                                   // max number of overcleavage
        if (   (sscanf(optarg, "%d", &param.maxOverCleavage) != 1)
            || (param.maxOverCleavage < 0)
            || (param.maxOverCleavage > 2)) {
          fprintf(stderr, "# Wrong value for maximum overcleavage\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'n':                                   // max number of modifications
        if (   (sscanf(optarg, "%d", &param.maxModif) != 1)
            || (param.maxModif < 0)) {
          fprintf(stderr, "# Wrong value for maximum number of modifications\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'o' :                                  // output (hits) filename
        (void) strcpy(outputFilename, optarg);
        if (! LXAssignToStdout(outputFilename)) {
          fprintf(stderr,"# Cannot create : %s\n", outputFilename);
          exit (LX_IO_ERROR);
        }
        break;

      case 'p' :                                  // report partial hits
        param.partialHit = TRUE;
        break;

      case 'v' :                                  // verbose mode
        param.verbose = TRUE;
        break;

      case 'y':                                   // met scan
        if (   (sscanf(optarg, "%d", &param.metScan) != 1)
            || (param.metScan < 0)) {
          fprintf(stderr, "# Wrong value for methionine scan\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;
  
      case 'S':
        if (   (sscanf(optarg,"%s", buffer) != 1)
            || (! (param.hitScoreType = sGetHitScoreType(buffer)))) {
          fprintf(stderr, "# Wrong value for hit score\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;
	
      case 's' :                                  // input (spectrum) filename
        (void) strcpy(spectrumFilename, optarg);
        break;
	
      case '?' :                                  // oops !
        ExitUsage(LX_ARG_ERROR);
        break;      
    }
  }

  /* --------------------------------- */
  /* check parameters                  */

  if (! *fastaFilename) {
    fprintf(stderr, "# You should provide a translated chromosome (fasta) filename ");
    fprintf(stderr, " by using -f option\n");
    ExitUsage(LX_ARG_ERROR);
  }

  if (! (fastaFile = LXOpenFile(fastaFilename, "r"))) {
    fprintf(stderr, "# Cannot access fasta file : %s\n", fastaFilename);
    ExitUsage(LX_ARG_ERROR);
  }

  if (LXAccess(aminoFilename, "r") != LX_NO_ERROR) {
    fprintf(stderr,"# Cannot access to aminoacid alphabet file : %s\n", aminoFilename);
    exit(LX_IO_ERROR);
  }

  if (LXAccess(enzymeFilename, "r") != LX_NO_ERROR) {
    fprintf(stderr,"# Cannot access to digestion enzyme file : %s\n", enzymeFilename);
    exit(LX_IO_ERROR);
  }

  if ((param.hitScoreType != PSTHitScoreType) && (! *spectrumFilename)) {
    fprintf(stderr, "# This score type requires a spectra data file\n");
    fprintf(stderr, "# please provide a spectra file by using -s option\n");
    ExitUsage(LX_ARG_ERROR);
  }

  assert(param.hitScoreType != UnknownHitScoreType);

  /* --------------------------------- */
  /* adjust arguments units            */
  
  if (param.massTolerance > 0)
    param.massTolerance = param.massTolerance * 1e-6;
  
  /* --------------------------------- */
  /* load aminoacid alphabet           */

  if (PMLoadAminoAlphabet(aminoFilename, &alpha) != LX_NO_ERROR) {
    fprintf(stderr,"# Cannot load alphabet from %s\n", aminoFilename);
    exit(LX_IO_ERROR);
  }

  /* --------------------------------- */
  /* load enzyme                       */

  if (PMLoadEnzyme(enzymeFilename, &enzyme) != LX_NO_ERROR) {
    fprintf(stderr,"# Cannot load digestion enzyme from %s\n", enzymeFilename);
    exit(LX_IO_ERROR);
  }

  /* --------------------------------- */
  /* start processing                  */
  /* --------------------------------- */

  StartupBanner(argc, argv);

  if (param.verbose) {
    fprintf(stderr, "# amino acid alphabet loaded from : %s\n", aminoFilename);
    fprintf(stderr, "# digestion enzyme loaded from : %s\n", enzymeFilename);
  }

  (void) LXStrCpuTime(TRUE);
  
  /* --------------------------------- */
  /* load tags                         */
  /* skip mark, read everything        */
  
  tagStack = LXNewStackp(LX_STKP_MIN_SIZE);
  
  if (PMReadTagStack(stdin, tagStack, TRUE)) {
    fprintf(stderr,"Cannot read tags\n");
    exit(LX_IO_ERROR);
  }
  
  nbTags = LXStackpSize(tagStack);

  if (param.verbose) {
    fprintf(stderr, "# %d tags loaded from : %s\n", 
                    nbTags, (*inputFilename ? inputFilename : "<stdin>"));
  }

  /* --------------------------------- */
  /* load spectra                      */
  
  if ( param.hitScoreType != PSTHitScoreType ) {
  
    if ( sReadSpectrumFile( spectrumFilename, &spectrumStack, spectrumReader ) != LX_NO_ERROR ) {
      fprintf(stderr,"Cannot read spectra\n");
      exit(LX_IO_ERROR);
    }
    nbSpectrums = LXStackpSize( spectrumStack );

    if (param.verbose) {
      fprintf(stderr, "# %d spectra loaded from : %s\n", 
                      nbSpectrums, spectrumFilename );
    }
  }

  
  /* --------------------------------- */
  /* init hashTable                    */

  if (! (kuple = UtilCheckTags(tagStack))) {
    fprintf(stderr, "# Tags should all have the same length\n");
    exit(LX_ARG_ERROR);
  }

  hashTable = HashNewTable(kuple);
  
  HashInitTable(hashTable, tagStack, param.differentiateKQ);

  
  /* --------------------------------- */
  /* loop on chromosome                */
  /* translation frames                */
  
  nbSeq = 0;
  
  fastaSeq = LXFastaNewSequence(BUFSIZ);
  
  while (LXFastaReadSequence(fastaFile, fastaSeq)) {
  
    nbSeq++;
    
    if (! (fastaSeq->ok)) {
      fprintf(stderr,"# Error reading sequence #%d\n", nbSeq);
      exit(LX_MEM_ERROR);
    }

    /* ------------------------------------ */ 
    /* check if sequence looks like protein */
    /* because its a common mistake to run  */
    /* PMMatch directly on DNA sequence     */
    /* (just check on first sequence)       */
    
    if ((nbSeq == 1) && (LXBioSeqType(fastaSeq->seq) == LX_BIO_SEQ_DNA)) {
        fprintf(stderr,"# WARNING : the sequences look like DNA\n");
        fprintf(stderr,"#           PMMatch works with proteic sequence(s)\n");
        fprintf(stderr,"#           Please use PMTrans before\n");
    }

    if (param.verbose) 
      fprintf(stderr, "# processing sequence: %d %s %s (%ld aa)\n", 
                      nbSeq, fastaSeq->name, fastaSeq->comment, fastaSeq->length);

    (void) UtilValidateSequence(fastaSeq->seq);

    /* ----------------------------- */
    /* fill hash table with kuples   */
    /* from sequence                 */
        
    HashFillTable(hashTable, fastaSeq->seq, kuple, param.differentiateKQ);

    /* ----------------------------- */
    /* loop on tags to :             */
    /*  - retrieve hits              */
    /*  - extend hits left and right */
    /*  - score hits                 */
    /*  - print hits                 */

    int itag;
    
    for (itag = 0 ; itag < nbTags ; itag++) {
    
      PMTag *tag = (PMTag *) LXStackpGetValue(tagStack, itag);
      
      // retrieve the spectrum associated with this PST
      if (    (param.hitScoreType != PSTHitScoreType)
           && (sRetrieveSpectrum(tag, spectrumStack) != LX_NO_ERROR)) {
        fprintf(stderr,"Cannot retrieve spectrum. The spectrum file may not correspond.\n");
        exit(LX_IO_ERROR);
      }
      
      PMHitStack *hitStack = HashLookupHits(hashTable, tag, param.differentiateKQ);
    
      // extend hits left and right
      (void) MassMatchHitStack(tag, hitStack, fastaSeq, &alpha, &enzyme, &param);
      
      // score hits 
      (void) ScoreHitStack(tag, hitStack, fastaSeq, &alpha, &enzyme, &param);
      
      // print hits
      // (void) LXReverseStackp(hitStack);
      (void) UtilPrintHitStack(tag, hitStack, fastaSeq);
      
      // free hits
      (void) PMFreeHitStack(hitStack);
    } 

    /* ----------------------------- */
    /* remove kuples from hashtable  */
    /* for next sequence             */
    
    HashFlattenTable(hashTable);
  }
  /* --------------------------------- */
  /* done: free memory                 */
  
  (void) fclose(fastaFile);
  
  (void) fprintf(stderr, "# number of sequence read : %d\n", nbSeq);

  (void) HashFreeTable(hashTable);

  (void) LXFastaFreeSequence(fastaSeq);
  
  (void) PMFreeTagStack(tagStack);

  fprintf(stderr,"# %s\n", LXStrCpuTime(FALSE));

  exit (0);
}











