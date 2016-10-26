/* test Random */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXRand.h"

#define MAX_BIN 10
#define NB_DRAW 100000

/* ------------------------------------------------------------ */
/* ugly test for LXRand											*/
/* ------------------------------------------------------------ */

static void sPrintDist(char *name, int *dist) {
   int i;
   float expect, chi;

   chi = 0.0;
   expect = (float) NB_DRAW / (float) MAX_BIN;

   printf("%15s :", name);
   for (i = 0 ; i < MAX_BIN ; i++) {
     float val = (float) dist[i] - expect;
     printf(" %10d", dist[i]);
     chi += (val * val) / expect;
   }
   printf(" chi=%.1f\n", chi);
}

static void sRazDist(int *dist) {
   int i;
   for (i = 0 ; i < MAX_BIN ; i++) {
     dist[i] = 0;
   }
}

int main() {

  int sel, i;
  int dist[MAX_BIN];
  
  LXRandomSelector select[7] = {
    LX_RAND_UNIX, LX_RAND_PARK_MILLER_BD,
    LX_RAND_LECUYER_BD, LX_RAND_KNUTH,
    LX_RAND_LCG_NUMREC, LX_RAND_LCG_CARBON,
    LX_RAND_LCG_JAVA

  };

  char *name[7] = {
    "Unix", "Park_Miller",
    "Lecuyer", "Knuth",
    "NumRec", "Carbon",
    "Java"
  };
  
  for (sel = 0 ; sel < 7 ; sel++) {
  
      sRazDist(dist);
      LXSelectRandom(select[sel]);
      LXInitRandom(0xffffff);
      
      for (i = 0 ; i < NB_DRAW ; i++) {
        dist[LXUniformIRandom(0, MAX_BIN)]++;
      }
      sPrintDist(name[sel], dist);
  }
  
  exit(0);
}
