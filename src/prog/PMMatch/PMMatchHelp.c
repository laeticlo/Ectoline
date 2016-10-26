/* -----------------------------------------------------------------------
 * $Id: PMMatchHelp.c 1366 2011-08-22 16:40:49Z viari $
 * -----------------------------------------------------------------------
 * @file: PMMatchHelp.c
 * @desc: PMMatch help functions 
 *
 * @history:
 * @+ <Wanou> : Jul 02 : first version
 * @+ <Gloup> : Aug 06 : complete code revision
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PepMap PMMatch help functions<br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX.h"
#include "PM.h"

#include "PMMatch.h"

/* ----------------------------------------------- */
/* print startup banner                            */
/* ----------------------------------------------- */

void StartupBanner(int argc, char *argv[])
{
  int i;
  fprintf(stderr,"# ------------------------------\n");
  fprintf(stderr,"# PepMap - PMMatch (MultiTag) - v %s\n", VERSION);
  fprintf(stderr,"#\n");
  fprintf(stderr,"# Map Peptide Sequence Tags on translated chromosome(s)\n");
  fprintf(stderr,"# ------------------------------\n");
  fprintf(stderr,"# run: ");
  for (i = 0 ; i < argc ; i++)
    fprintf(stderr,"%s ", argv[i]);
  fprintf(stderr,"\n");
  fprintf(stderr,"# ------------------------------\n");
}

/* ----------------------------------------------- */
/* print usage and exit(stat)                      */
/* ----------------------------------------------- */

void ExitUsage(int stat)
{
  fprintf(stderr,"Usage: PMMatch [options]\n");
  fprintf(stderr,"type PMMatch -h for help\n");
  exit(stat);
}

/* ----------------------------------------------- */
/* print Help                                      */
/* ----------------------------------------------- */

void PrintHelp()
{
  fprintf(stderr,"Usage: PMMatch [options]\n");
  fprintf(stderr," options:\n");
  fprintf(stderr," -a AminoAcidFile : aminoacid reference file\n");
  fprintf(stderr,"                    (default = \"%s\")\n", DFT_AA_FILENAME);
  fprintf(stderr," -d tolerance     : mass tolerance in ppm\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_TOLERANCE);
  fprintf(stderr," -D tolerance     : mass tolerance in amu\n");
  fprintf(stderr,"                    (default = see -d\n");
  fprintf(stderr," -e EnzymeFile    : digestion enzyme reference file\n");
  fprintf(stderr,"                    (default = \"%s\")\n", DFT_ENZ_FILENAME);
  fprintf(stderr," -f FastaFile     : translated chromosome filename\n");
  fprintf(stderr,"                    (default = NONE) *you should provide this filename*\n");
  fprintf(stderr," -F pkl | PKL | dta  : spectrum format\n");
  fprintf(stderr,"                    (default = pkl)\n");
  fprintf(stderr," -h               : print this help\n");
  fprintf(stderr," -i PSTFile       : PSTs filename\n");
  fprintf(stderr,"                    (default = <stdin>)\n");
  fprintf(stderr," -k               : differentiate between K/Q\n");
  fprintf(stderr,"                    (default = off)\n");
  fprintf(stderr," -m miscleavage   : maximum number of miscleavage(s)\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_MAX_MISCLEAVAGE);
  fprintf(stderr," -M overcleavage  : maximum number of overcleavage(s) (0, 1 or 2)\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_MAX_OVERCLEAVAGE);
  fprintf(stderr," -n maxModif      : maximum number of modified residues\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_MAX_MODIF);
  fprintf(stderr," -o outputFile    : output (hits) filename\n");
  fprintf(stderr,"                    (default = <stdout>)\n");
  fprintf(stderr," -p               : report partial hits too\n");
  fprintf(stderr,"                    (default = off)\n");
  fprintf(stderr," -S (P|R|S|D)     : type of hit score\n");
  fprintf(stderr,"                    (P = pst, R = remt, S = sumrank, D = dancink99)\n");
  fprintf(stderr,"                    note: all score types (except P) need the -s option too\n");
  fprintf(stderr,"                    (default = P)\n");
  fprintf(stderr," -s spectrumFile  : spectrum file\n");
  fprintf(stderr,"                    you should provide a spectrum file for all score types except P\n");
  fprintf(stderr,"                    (default = NONE)\n"); 
  fprintf(stderr," -v               : verbose mode (add some internal information)\n");
  fprintf(stderr,"                    (default = off)\n");
  fprintf(stderr," -y metBefore     : number of positions to scan for Nterm Methionine\n");
  fprintf(stderr,"                    0 means no Nterm scan\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_MET_SCAN);
  fprintf(stderr,"\n");
  fprintf(stderr,"note 1: -S (and -s) options is still *highly* experimental\n");
  fprintf(stderr,"\n");

}
