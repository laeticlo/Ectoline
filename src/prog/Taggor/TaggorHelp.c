/* -----------------------------------------------------------------------
 * $Id: TaggorHelp.c 1366 2011-08-22 16:40:49Z viari $
 * -----------------------------------------------------------------------
 * @file: TaggorHelp.c
 * @desc: Taggor help functions
 *
 * @history:
 * @+ <romain>  : Feb 03 : first version
 * @+ <Gloup>   : Dec 2005 : major revision
 * @+ <Gloup>   : Aug 2006 : code cleanup
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * Taggor help functions<br>
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
/* print parameters								   */
/* ----------------------------------------------- */

void PrintParameters(Parameter *param) {
  fprintf(stderr,"# ------------------------------\n");
  fprintf(stderr,"# Parameters\n");
  fprintf(stderr,"# ------------------------------\n");
  fprintf(stderr,"# oligoLength    : %d\n", param->oligoLength);
  fprintf(stderr,"# nbResults      : %d\n", param->nbResults);
  fprintf(stderr,"# statESTParam   : %d\n", param->statESTParam);
  fprintf(stderr,"# statRParam     : %d\n", param->statRParam);
  fprintf(stderr,"# rankThreshold  : %d\n", param->rankThreshold);
  fprintf(stderr,"# peakThreshold  : %f\n", param->peakThreshold);
  fprintf(stderr,"# massThreshold  : %f\n", param->massThreshold);
  fprintf(stderr,"# massTolerance  : %f\n", param->massTolerance);
  fprintf(stderr,"# minScore       : %f\n", param->minScore);
  fprintf(stderr,"# denovo         : %s\n", param->denovo ? "on" : "off");
  fprintf(stderr,"# denovoRmin     : %f\n", param->denovoRmin);
  fprintf(stderr,"# tagCorrelation : %s\n", param->tagCorrelation ? "on" : "off");
  fprintf(stderr,"# verbose        : %s\n", param->verbose ? "on" : "off");
  fprintf(stderr,"# statMode       : %d\n", param->statMode);
  fprintf(stderr,"# uniqueMode     : %d\n", param->uniqueMode);
  fprintf(stderr,"#\n");
}

/* ----------------------------------------------- */
/* print startup banner                            */
/* ----------------------------------------------- */

void StartupBanner(int argc, char *argv[])
{
  int i;
  fprintf(stderr,"# ------------------------------\n");
  fprintf(stderr,"# Taggor v %s\n", VERSION);
  fprintf(stderr,"#\n");
  fprintf(stderr,"# Generate Peptide Sequence Tags (PSTs) from mass spectra\n");
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
  fprintf(stderr,"Usage: Taggor [options]\n");
  fprintf(stderr,"type Taggor -h for help\n");
  exit(stat);
}

/* ----------------------------------------------- */
/* print Help                                      */
/* ----------------------------------------------- */

void PrintHelp()
{
  fprintf(stderr,"Taggor v %s\n", VERSION);
  fprintf(stderr,"Usage: Taggor [options]\n");
  fprintf(stderr," options:\n");
  fprintf(stderr," -a aminoAcidFile : aminoacid reference file\n");
  fprintf(stderr,"                    (default = \"%s\")\n", DFT_AA_FILENAME);
  fprintf(stderr," -c               : turn ON tag correlation mode in score calculation\n");
  fprintf(stderr,"                    (default = OFF)\n");
  fprintf(stderr," -d tolerance     : mass tolerance in ppm\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_TOLERANCE);
  fprintf(stderr," -D tolerance     : mass tolerance in amu\n");
  fprintf(stderr,"                    (default = see -d)\n");
  fprintf(stderr," -F pkl | PKL | dta : spectrum format\n");
  fprintf(stderr,"                    (default = pkl)\n");
  fprintf(stderr," -h               : print this help\n");
  fprintf(stderr," -i SpectrumFile  : spectrum filename\n");
  fprintf(stderr,"                    (default = <stdin>)\n");
  fprintf(stderr," -l tagLength     : length of tags\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_OLIGO_LENGTH);
  fprintf(stderr," -m threshold     : spectrum low mass threshold\n");
  fprintf(stderr,"                    (default = %f)\n", DFT_MASS_THRESHOLD);
  fprintf(stderr," -n param         : set denovo parameter\n");
  fprintf(stderr,"                    use <= 0 to turn off denovo calculation\n");
  fprintf(stderr,"                    (default = %.2f)\n", DFT_DENOVO_RMIN);
  fprintf(stderr," -o outputFile    : output (usually PST) filename\n");
  fprintf(stderr,"                    (default = <stdout>)\n");
  fprintf(stderr,"                    note : with the -R option this file contains\n");
  fprintf(stderr,"                           the recalibrated spectra\n");
  fprintf(stderr," -r nbResult      : max number of PSTs per spectrum\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_NB_RES);
  fprintf(stderr," -R               : recalibrate spectra using tags\n");
  fprintf(stderr,"                    (default = off)\n");
  fprintf(stderr,"                    note : with this option no tags are printed,\n");
  fprintf(stderr,"                           the output file contains the recalibrated spectra\n");
  fprintf(stderr," -s zscore        : minimum PSTs score\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_MIN_SCORE);
  fprintf(stderr," -S R | E | S | T : score statistics\n");
  fprintf(stderr,"                    R : rank statistics\n");
  fprintf(stderr,"                    E : empirical PST distribution\n");
  fprintf(stderr,"                    S : shuffled spectrum distribution\n");
  fprintf(stderr,"                    T : theoretical spectrum distribution\n");
  fprintf(stderr,"                    (default = R)\n");
  fprintf(stderr,"                    note : R|E|S|T can be immediately (no space)\n");
  fprintf(stderr,"                           followed by digits interpreted as\n");
  fprintf(stderr,"                           a statistic optional parameter\n");
  fprintf(stderr,"                           default for R = %d\n", DFT_STAT_R_PARAM);
  fprintf(stderr,"                           default for EST = %d\n", DFT_STAT_EST_PARAM);
  fprintf(stderr," -t threshold     : spectrum intensity threshold (in %%)\n");
  fprintf(stderr,"                    (default = %f)\n", DFT_PEAK_THRESHOLD);
  fprintf(stderr," -T threshold     : spectrum rank threshold (<=0 means no threshold)\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_RANK_THRESHOLD);
  fprintf(stderr," -u               : unique PST mode\n");
  fprintf(stderr,"                    remove duplicate PST per spectrum\n");
  fprintf(stderr,"                    (default = off)\n");
  fprintf(stderr," -U               : unique lexicon mode\n");
  fprintf(stderr,"                    remove duplicate lexicon per spectrum\n");
  fprintf(stderr,"                    (default = off)\n");
  fprintf(stderr," -v               : verbose mode (add some internal information)\n");
  fprintf(stderr,"                    (default = off)\n");
  fprintf(stderr,"\n");
  fprintf(stderr," note 1: -S and -c options are still *highly* experimental\n");
  fprintf(stderr," note 2: spectrum formats are\n");
  fprintf(stderr," pkl : standard pkl format (1rst line (parent) is [M+zH]/z intensity z\n");
  fprintf(stderr," PKL : modified  pkl format (1rst line (parent) is [M+H] intensity z\n");
  fprintf(stderr," dta : mono pkl format (1rst line (parent) is [M+H] intensity z\n");
}

