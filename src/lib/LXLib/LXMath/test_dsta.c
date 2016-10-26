/* test LXDStat */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "LX/LXMath.h"

/* ------------------------------------------------------------ */
/* simple test for LXDStat					        			*/
/* ------------------------------------------------------------ */

int main() {
  size_t n = 0;
  float data[1024];
  LXStat stat;
  
  LXInitStat(&stat);
  
  while (scanf("%f", data + n) == 1) {
    LXUpdateStat(&stat, data[n]);
    n++;
  }
  
  LXComputeStat(&stat);
  
  printf("%zu %.3f %.6f %.6f\n", n, LXMeanF(data, n), 
                                    LXVarianceF(data, n), 
                                    LXStdDeviationF(data, n));  

  printf("%zu %.3f %.6f %.6f\n", stat.npt, stat.mean, stat.var, stat.stdev);
  
  printf("%.3f %.3f %.3f\n", LXQuantileF(data, n, 0.25, FALSE),
                             LXQuantileF(data, n, 0.50, FALSE),
                             LXQuantileF(data, n, 0.75, FALSE));
  exit(0);
}
