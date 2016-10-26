/* test job completion notifier */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "LX/LXUtils.h"

#define N 100
//#define M 1000000
#define M 200000

static double sFakeComputation() {
    int j;
    double s = 0;
    for (j = 0 ; j < M ; j++) {
      s += fabs(cos(log(j+1)));
    }
    return s;
}

/* -------------------------------------------- */

main() {

  int i;

  char info1[64], info2[64]; 
  char *infoArray[3] = {info1, info2, NULL};

  LXComplPtr comp = LXNewCompleter(LX_COMPLETER_AUTO, "Test", N, 
                                   "units", 500, infoArray);

  for (i = 0 ; i <= N ; i++) {

    (void) sprintf(info1, "User info1 = %d", i*10);
    (void) sprintf(info2, "User info2 = %s", LXStrCurrentTime());

    LXDoCompleter(comp, i);

    double s = sFakeComputation();
    
    if (s < 0)
      printf("%d %f\n", i, s);
  }
  
  LXEndCompleter(comp);

  exit(0);
}

