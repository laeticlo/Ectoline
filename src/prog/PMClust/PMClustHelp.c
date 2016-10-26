/* -----------------------------------------------------------------------
 * $Id: PMClustHelp.c 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMClustHelp.c
 * @desc: PMClust help functions 
 *
 * @history:
 * @+ <Wanou> : Jul 02 : first version
 * @+ <Gloup> : Aug 06 : complete code revision
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PepMap PMClust help functions<br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX.h"
#include "PM.h"

#include "PMClust.h"

/* ----------------------------------------------- */
/* print startup banner                            */
/* ----------------------------------------------- */

void StartupBanner(int argc, char *argv[])
{
  int i;
  fprintf(stderr,"# ------------------------------\n");
  fprintf(stderr,"# PepMap - PMClust- v %s\n", VERSION);
  fprintf(stderr,"#\n");
  fprintf(stderr,"# Clusterize hits on chromosome(s) or protein(s)\n");
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
  fprintf(stderr,"Usage: PMClust [options]\n");
  fprintf(stderr,"type PMClust -h for help\n");
  exit(stat);
}

/* ----------------------------------------------- */
/* print Help                                      */
/* ----------------------------------------------- */

void PrintHelp()
{
  fprintf(stderr,"Usage: PMClust [options]\n");
  fprintf(stderr," options:\n");
  fprintf(stderr," -d distance      : clustering distance parameter (in nucleotides)\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_DISTANCE);
  fprintf(stderr," -f               : force to cluster hits in the same frame\n");
  fprintf(stderr,"                    (this is used for procaryota only)\n");
  fprintf(stderr,"                    (default = off)\n");
  fprintf(stderr," -h               : print this help\n");
  fprintf(stderr," -i HitFile       : Hits filename\n");
  fprintf(stderr,"                    (default = <stdin>)\n");
  fprintf(stderr," -o outputFile    : output (cluster) filename\n");
  fprintf(stderr,"                    (default = <stdout>)\n");
  fprintf(stderr," -p               : add partial hits in clusters\n");
  fprintf(stderr,"                    (default = off)\n");
  fprintf(stderr," -P               : cluster in Protein mode\n");
  fprintf(stderr,"                    (default = Nucleic mode)\n");
  fprintf(stderr," -t threshold     : report only clusters with at least threshold full hits\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_MIN_FULL);
  fprintf(stderr," -T threshold     : report only cluster with at least threshold pephits\n");
  fprintf(stderr,"                    (default = %d)\n", DFT_MIN_PEPHITS);
  fprintf(stderr," -v               : verbose mode (add some internal information)\n");
  fprintf(stderr,"                    (default = off)\n");
  fprintf(stderr,"\n");
  fprintf(stderr,"note: in Protein mode (-P) , parameters -d and -f are ignored \n");
  fprintf(stderr,"\n");
}
