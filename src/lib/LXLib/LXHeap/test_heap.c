/* test Heap */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXHeap.h"

/* ------------------------------------------------------------ */
/* ugly test for LXHeap											*/
/* ------------------------------------------------------------ */


typedef struct {
   int indx;
   int score;
   int iamfree;
} MyStruct;

void myStructFree(void *x) { // not a real free : just test
  MyStruct *s = (MyStruct *) x;
  if (s->iamfree) {
    printf("ERROR free mystruct %d\n", s->indx);
    exit(1);
  }
  s->iamfree = 1;
}

int myStructCompar(const void *x, const void *y) {
  MyStruct * xx = (MyStruct *) x;
  MyStruct * yy = (MyStruct *) y;
  return (xx->score - yy->score);
}


int main() {

  int i, j, k;
  LXHeap *h;
  MyStruct *pk, *prev, keep[10000];
  
  srandom(0);

  for (i = 1 ; i < 100 ; i++) {
  
    h = LXNewHeap(i, myStructCompar, myStructFree);
    
    for (j = 0 ; j < 10 * i ; j++) {
      k = (random() / 10) + 1;
      keep[j].indx = j;
      keep[j].score = k;
      keep[j].iamfree = 0;
      LXAddInHeap(h, (void *) (keep + j));
    }

    prev = (MyStruct *) LXPopHeapRoot(h);
    while (pk = (MyStruct *) LXPopHeapRoot(h)) {
      if (pk->score < prev->score) {
         printf("ERROR %d %d", pk->score, prev->score);
         exit(1);
      }
      myStructFree(prev);
      prev = pk;
    }

    myStructFree(prev);

    for (j = 0 ; j < 10 * i ; j++) {
      if (! keep[j].iamfree) {
        printf("ERROR not free (%d) %d %d\n", (10*i), keep[j].indx, j);
        exit(1);
      }
    }

    LXFreeHeap(h);
  }
  
  printf("test heap OK\n");

  return 0;
}
