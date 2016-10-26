/* test LXMulti */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "LX/LXMath.h"

int main() {

  int i, N=10, M=5, n=6;
  double sum = 0;
  
  LXSetup(); // force posix behavior for "%g" on win32
  
  for (i = 0; i <= M ; i++) {
    double proba = exp(LXLogHyperPMF(N, n, M, i));
    printf("%d %.2f\n", i, proba);
    sum += proba;
  }
  
  printf("sum = %.2f\n", sum);

  double pleft, pright, pobs;
  
  (void) LXFisherOneTail(10, 10, 10, 10,  
                        &pleft, &pright, &pobs);
  printf("%.2f %.2f %.2f\n", pobs, pleft, pright);                        

  (void) LXFisherOneTail(5, 10, 10, 10,  
                        &pleft, &pright, &pobs);
  printf("%.2f %.2f %.2f\n", pobs, pleft, pright);                        

  (void) LXFisherOneTail(10000, 10000, 10000, 10000,  
                        &pleft, &pright, &pobs);
  printf("%.2f %.2f %.2f\n", pobs, pleft, pright);                        

  (void) LXFisherOneTail(1000, 2000, 3000, 4000,  
                        &pleft, &pright, &pobs);
  printf("%.2g %.2g %.2g\n", pobs, pleft, pright);                        

  exit(0);
}
