/* test LXMulti */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "LX/LXMath.h"

typedef struct {
    double *proba;
    double sum;
} sMulti;

typedef struct {
    double proba;
    double sum;
} sBino;

static int myPrintMulti(int n, int k, const int *m, void *arg) {
    sMulti *multi = (sMulti *) arg;
    int i;
    double proba = LXLogMultinomialPMF(k, m, multi->proba);
    for (i = 0 ; i < k ; i++)
        printf("%d ", m[i]);
    printf(": %.2f\n", proba);
    multi->sum += exp(proba);
    return 1;
}

static int myPrintBino(int n, int k, void *arg) {
    sBino *bino = (sBino *) arg;
    int i;
    double proba = LXLogBinomialPMF(n, k, bino->proba);
    printf("%d %d : %.2f\n", k, n-k, proba);
    bino->sum += exp(proba);
    return 1;
}

int main() {

  double p[3] = {0.5, 0.3, 0.2};
  sMulti multi = {p, 0.};

  LXMultinomialGenerator(5, 3, myPrintMulti, &multi);

  printf("sum = %.2f\n", multi.sum);

  sBino bino = {0.9, 0.};
  
  int i;
  for (i = 10 ; i >= 0 ; i--) {
    (void) myPrintBino(10, i, &bino);
  }

  printf("sum = %.2f\n", bino.sum);

  exit(0);
}
