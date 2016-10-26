/* test Random */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "LX/LXRand.h"

#define NB_BITS sizeof(long) * 8
#define NB_DRAW 100000

/* ------------------------------------------------------------ */
/* ugly test for LXRand											*/
/* ------------------------------------------------------------ */

static void sPrintDist(char *name, int *dist) {
   int i;
   float expect, chi;

   chi = 0.0;
   expect = 0.5;

   printf("%15s :", name);
   for (i = 0 ; i < NB_BITS ; i++) {
     float freq = (float) dist[i] / (float) NB_DRAW;
     printf(" %4.2f", freq);
     chi += ((freq - expect) * (freq - expect)) / expect;
   }
   printf(" chi=%.1f\n", chi);
}

static void sRazDist(int *dist) {
   int i;
   for (i = 0 ; i < NB_BITS ; i++) {
     dist[i] = 0;
   }
}

static void sCumulDist(int *dist, long value) {
    int i;
    long mask = 0x1;
    for (i = 0 ; i < NB_BITS ; i++) {
      if (value & mask)
        dist[i]++;
      mask <<= 1;
    }
}

int main() {

  int sel, i;
  int dist[NB_BITS];
  
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
        sCumulDist(dist, LXUniformIRandom(0, LONG_MAX));
      }
      sPrintDist(name[sel], dist);
  }
  
  exit(0);
}
