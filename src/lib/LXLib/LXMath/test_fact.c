/* test LXFact */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "LX/LXMath.h"

static UInt64 myFact(int n) {
  return (n <= 1 ? 1 : n * myFact(n-1));
}

static int myPrint(int n, const int *s, void *arg) {
    int i;
    for (i = 0 ; i < n ; i++)
        printf("%d ", s[i]);
    printf("\n");
    (*((int *) arg))++;
    return 1;
}

int main() {

  int i, n;
  
  for (i = 0 ; i <= 16 ; i++) {
    double y = round(exp(LXLogFactorial(i)));
    UInt64 f = myFact(i);
    printf("%d %.0f %llu\n", i, y, f);
    if (y != f) exit(1);
  }


  n = 0;
  
  LXPermutationGenerator(5, myPrint, &n);

  printf("%d\n", n);
  
  exit(0);
}
