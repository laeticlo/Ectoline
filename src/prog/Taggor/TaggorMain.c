/* -----------------------------------------------------------------------
 * $Id: TaggorMain.c 1807 2013-02-24 23:26:33Z viari $
 * -----------------------------------------------------------------------
 * @file: TaggorMain.c
 * @desc: Taggor main program
 *
 * @history:
 * @+ <romain>  : Feb 03 : first version
 * @+ <Gloup>   : Dec 2005 : major revision
 * @+ <Gloup>   : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * Taggor main program<br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX.h"
#include "PM.h"

#include "Taggor.h"

/* ----------------------------------------------- */
static StatMode sGetStatMode(char *mode)
{
   if (*mode == 'R')
     return RankStat;

   if (*mode == 'E')
     return EmpiricalStat;

   if (*mode == 'S')
     return SpectrumStat;

   if (*mode == 'T')
     return TheoreticalStat;
     
   return UnknownStat;
}

/* ----------------------------------------------- */
static int sGetStatParam(char *mode, int *arg)
{
   if (mode[1] == '\000')
     return LX_NO_ERROR;
     
   if (sscanf(mode+1, "%d", arg) != 1)
     return LX_ARG_ERROR;

   if (*arg <= 0)
     return LX_ARG_ERROR;

   return LX_NO_ERROR;
}

/* ----------------------------------------------- */
/* main entry point                                */
/* ----------------------------------------------- */

main(int argc, char *argv[])
{
  extern char *optarg;

  int opt, statParam, nbSpectra;
  
  char aminoFileName[BUFSIZ], tagFileName[BUFSIZ],
       spectrumFileName[BUFSIZ], spectrumBaseName[BUFSIZ],
       buffer[BUFSIZ];
  
  LXTmpFile tmpFile;
  
  LXStat dmmStat;
       
  PMAminoAlphabet alpha;
  
  PMSpectrum *spRaw;
  
  Parameter param;
  
  PMSpectrumReader spectrumReader;
  PMSpectrumWriter spectrumWriter;
  
  /* -------------------------- */
  /* setup default parameters   */
  
  param.oligoLength        = DFT_OLIGO_LENGTH;
  param.nbResults          = DFT_NB_RES;
  param.massTolerance      = DFT_TOLERANCE;
  param.peakThreshold      = DFT_PEAK_THRESHOLD;
  param.rankThreshold      = DFT_RANK_THRESHOLD;
  param.massThreshold      = DFT_MASS_THRESHOLD;
  param.statESTParam       = DFT_STAT_EST_PARAM;
  param.statRParam         = DFT_STAT_R_PARAM;
  param.minScore           = DFT_MIN_SCORE;
  param.denovoRmin         = DFT_DENOVO_RMIN;
  param.statMode           = RankStat;
  param.uniqueMode         = NoUnique;
  param.verbose            = FALSE;
  param.recalibrate        = FALSE;
  param.denovo             = FALSE;
  param.tagCorrelation     = FALSE;

  (void) strcpy(aminoFileName, DFT_AA_FILENAME);

  spectrumReader = PMReadPKL1Spectrum;
  spectrumWriter = PMWritePKL1Spectrum;
  
  *spectrumFileName='\000';
  
  /* -------------------------- */
  /* get user's parameters      */

  while ((opt = getopt(argc, argv, "a:cd:D:F:Hhi:l:m:n:o:r:Rs:S:t:T:uUv")) != -1) {
  
    switch (opt) {
      
      case 'a' :                                  // Amino-acid filename
        (void) strcpy(aminoFileName, optarg);
        break;
  
      case 'c' :                                  // use tag correlation
        param.tagCorrelation = TRUE;
        break;
  
      case 'd':                                   // mass tolerance (ppm)
        if (   (sscanf(optarg, "%f", &param.massTolerance) != 1)
            || (param.massTolerance <= 0)) {
          fprintf(stderr, "# Wrong value for mass tolerance\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'D':                                   // mass tolerance (uma)
        if (   (sscanf(optarg, "%f", &param.massTolerance) != 1)
            || (param.massTolerance <= 0)) {
          fprintf(stderr, "# Wrong value for mass tolerance\n");
          ExitUsage(LX_ARG_ERROR);
        }
        param.massTolerance = -param.massTolerance; // convention
        break;

  
      case 'F':                                   // input data format
        if (   (sscanf(optarg, "%s", buffer) != 1) 
            || (! (spectrumReader = PMGetSpectrumReader(buffer)))
            || (! (spectrumWriter = PMGetSpectrumWriter(buffer)))) {
          fprintf(stderr, "# Unknown data format %s\n", buffer);
          ExitUsage(LX_ARG_ERROR);
        }
        break;
  
      case 'H' :                                  // help
      case 'h' :
        PrintHelp();
        exit(0);
        break;
        
      case 'i' :                                  // input (spectrum) filename
        (void) strcpy(spectrumFileName, optarg);
        if(! LXAssignToStdin(spectrumFileName)) {
          fprintf(stderr, "# Cannot open : %s\n", spectrumFileName);
          exit (LX_IO_ERROR); 
        }
        break;

      case 'l' :                                  // oligo length
        if (   (sscanf(optarg, "%d", &param.oligoLength) != 1)
            || (param.oligoLength <= 0)
            || (param.oligoLength > 8)) {
          fprintf(stderr, "# Wrong value for oligo length\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;
  
      case 'm':                                   // spectrum low mass threshold
        if (sscanf(optarg, "%f", &param.massThreshold) != 1) {
          fprintf(stderr, "# Wrong value for low mass threshold\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'n' :                                  // denovo sequencing parameter
        if (sscanf(optarg, "%f", &param.denovoRmin) != 1) {
          fprintf(stderr, "# Wrong value for denovo rmin parameter\n");
          ExitUsage(LX_ARG_ERROR);
        }
        param.denovo = (param.denovoRmin > 0);
        break;
        
      case 'o' :                                  // output (tag) filename
        (void) strcpy(tagFileName, optarg);
        if (! LXAssignToStdout(tagFileName)) {
          fprintf(stderr, "# Cannot create : %s\n", tagFileName);
          exit (LX_IO_ERROR);
        }
        break;
  
      case 'r':                                   // max number of results
        if (   (sscanf(optarg, "%d", &param.nbResults) != 1)
            || (param.nbResults <= 0)) {
          fprintf(stderr, "# Wrong value for number of results\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'R' :                                  // recalibration mode
        param.recalibrate = TRUE;
        break;

      case 's':                                   // score threshold
        if (sscanf(optarg, "%f", &param.minScore) != 1) {
          fprintf(stderr, "# Wrong value for zscore threshold\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;
  
      case 'S':                                   // stat mode
      {
        int statParam = 0;
        if (   (sscanf(optarg,"%s", buffer) != 1) 
            || ((param.statMode = sGetStatMode(buffer)) == UnknownStat)
            || (sGetStatParam(buffer, &statParam) != LX_NO_ERROR)) {
          fprintf(stderr, "# Wrong value for statistics\n");
          ExitUsage(LX_ARG_ERROR);
        }
        if (statParam <= 0)
          break;
        if (param.statMode == EmpiricalStat)
          LX_WARNING("value ignored for E statistics");
        else if (param.statMode == RankStat)
          param.statRParam = statParam;
        else
          param.statESTParam = statParam;
        break;
      }

      case 't':                                   // spectrum intensity threshold
        if (   (sscanf(optarg, "%f", &param.peakThreshold) != 1)
            || (param.peakThreshold <= 0)) {
          fprintf(stderr, "# Wrong value for peak intensity threshold\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;
  
      case 'T':                                   // spectrum rank threshold
        if (sscanf(optarg, "%d", &param.rankThreshold) != 1) {
          fprintf(stderr, "# Wrong value for peak rank threshold\n");
          ExitUsage(LX_ARG_ERROR);
        }
        break;

      case 'u' :                                  // unique tag mode
        param.uniqueMode = UniqueTag;
        break;

      case 'U' :                                  // unique lexicon mode
        param.uniqueMode = UniqueSeq;
        break;

      case 'v' :                                  // verbose mode
        param.verbose = TRUE;
        break;
  
      case '?' :                                  // oops !
        ExitUsage(LX_ARG_ERROR);
        break;      
    }
  }

  /* -------------------------- */
  /* adjust arguments units     */
  
  if (param.massTolerance >= 0)     // relative ppm
    param.massTolerance = param.massTolerance * 1e-6; 

  param.peakThreshold *= 1e-2;

  (void) strcpy(spectrumBaseName, 
               (*spectrumFileName ? LXBaseName(spectrumFileName, TRUE) : "stdin"));

  /* -------------------------- */
  /* load aminoacid alphabet    */
  
  if (LXAccess(aminoFileName, "r") != LX_NO_ERROR) {
    fprintf(stderr, "# Cannot access to aminoacid alphabet file : %s\n", aminoFileName);
    exit(LX_IO_ERROR);
  }
  
  if (PMLoadAminoAlphabet(aminoFileName, &alpha) != LX_NO_ERROR) {
    fprintf(stderr, "# Cannot load alphabet from %s\n", aminoFileName);
    exit(LX_IO_ERROR);
  }

  /* -------------------------- */
  /* for recalibration: open    */
  /* temporary file for spectra */
  
  if (param.recalibrate) {
    if (LXOpenTmpFile(&tmpFile) != LX_NO_ERROR) {
      fprintf(stderr, "# Cannot open temporary file %s\n", tmpFile.fileName);
      exit(LX_IO_ERROR);
    }
  }
  
  /* -------------------------- */
  /* start processing spectra   */

  StartupBanner(argc, argv);

  if (param.verbose)
    PrintParameters(&param);
  
  if (param.verbose) {
    fprintf(stderr, "# amino acid alphabet loaded from : %s\n", aminoFileName);
  }

  (void) LXStrCpuTime(TRUE);

  LXInitStat(&dmmStat);
  
  spRaw = PMNewSpectrum();
  
  spRaw->dmm = param.massTolerance;

  nbSpectra = 0;

  LXSetupIO(spectrumBaseName);

  fprintf(stderr, "# Processing spectra\n");

  while (spectrumReader(stdin, spRaw) == LX_NO_ERROR) {
  
    PMSpectrum *spFiltered;
    PMTagStack *stkRes;
    
    nbSpectra++;
    
    fprintf(stderr, "# processing spectrum %d\r", nbSpectra);
    if (param.verbose) 
      fprintf(stderr, "\n");

    spFiltered = PreprocessSpectrum(spRaw, &alpha, &param);

    (void) sprintf(spFiltered->id,"%s.%d", spectrumBaseName, nbSpectra);

    if (param.verbose) {
      fprintf(stderr, "# filtered spectrum\n");
      (void) PMWriteDTA1Spectrum(stderr, spFiltered);
    }

    stkRes = ComputeTags(spFiltered, &alpha, &param, &dmmStat);

								/* ------------------------------------ */
    if (param.recalibrate) {    /* just keep current spectrum for       */
    							/* further recalibration 				*/
      spectrumWriter(tmpFile.stream, spRaw);
    }

								/* ------------------------------------ */
    else {                      /* true output of PST's                 */
    
      fprintf(stdout,"# Tags generated from %s (mass = %f)\n",
              spFiltered->id, spFiltered->parent.mass);

      PMWriteTagStack(stdout, stkRes);
    }

    PMFreeTagStack(stkRes);
    PMFreeSpectrum(spFiltered);

    spRaw->nbPeaks = 0; // reset raw spectrum
  }

  if (! param.verbose) 
    fprintf(stderr, "\n");

  LXComputeStat(&dmmStat);

  if (param.verbose) {
    fprintf(stderr, "# PPM statistics : n= %ld mean= %.6f stdev= %.6f\n",
                    dmmStat.npt, dmmStat.mean, dmmStat.stdev);
  }

  /* -------------------------- */
  /* recalibration mode         */

  if (param.recalibrate) {
    fprintf(stderr, "# Recalibration : n= %ld ppm= %.6f stdev= %.6f\n",
                    dmmStat.npt, dmmStat.mean, dmmStat.stdev);
                    
    fclose(tmpFile.stream);
    
    LXAssignToStdin(tmpFile.fileName);
    
    while (spectrumReader(stdin, spRaw) == LX_NO_ERROR) {
    
      PMSpectrum *spCalibrated = PMRecalibrateSpectrum(spRaw, -dmmStat.mean / 1000000.);
      
      (void) spectrumWriter(stdout, spCalibrated);
      
      PMFreeSpectrum(spCalibrated);
      
      spRaw->nbPeaks = 0; // reset raw spectrum
    }
  }
  
  PMFreeSpectrum(spRaw);

  fprintf(stderr, "# CpuTime: %s\n", LXStrCpuTime(TRUE));
  
  exit(0);

  /* never reached */
}
