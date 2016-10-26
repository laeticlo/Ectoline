/* test Dict */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXDict.h"

#define SIZE 10000

#if (SIZE < 1000) 
#define PRINT(fmt, arg1, arg2) printf(fmt, arg1, arg2)
#else
#define PRINT(fmt, arg1, arg2)
#endif

/* ------------------------------------------------------------ */
/* ugly test for dictionnary									*/
/* ------------------------------------------------------------ */


typedef struct {
   int score;
   int iamfree;
} MyStruct;

void myStructFree(void *x) {
  MyStruct *s = (MyStruct *) x;
  if (s->iamfree) {
    printf("ERROR free mystruct\n");
    exit(1);
  }
  s->iamfree = 1;
}

int myStructCompar(const void *x, const void *y) {
  MyStruct * xx = (MyStruct *) x;
  MyStruct * yy = (MyStruct *) y;
  return (xx->score - yy->score);
}

void myAction(void *x, void *arg) {
/*ARGSUSED*/
  MyStruct * xx = (MyStruct *) x;
  PRINT("action score: %d iamfree: %d\n", xx->score, xx->iamfree);
}

void sPrintDict(LXDict *dict) {
  LXArray *arr = LXDictToArray(dict);
  Ptr *ptr = (Ptr *) arr->data;
  int i;
  for (i = 0 ; i < arr->nbelt ; i++) {
    MyStruct *x = (MyStruct *) ptr[i];
    PRINT("score: %d iamfree: %d\n", x->score, x->iamfree);
  }
  LXFreeArray(arr);
}

int main() {

  int i, k;
  LXDict *dict;
  MyStruct keep[SIZE], look;
  char check[2*0xffff + 2];

  for (i = 0 ; i <= 2 * 0xffff ; i++) {
    check[i] = 0;
  }
  
  dict = LXNewDict(myStructCompar);
  
  srandom(0);

  for (i = 0 ; i < SIZE ; i++) {
  
    k = random() & 0xffff;
    
    keep[i].score = 2 * k;
    keep[i].iamfree = 0;

    check[2 * k] = 1;
        
    PRINT("insert %d %d\n", i, keep[i].score);
    
    if (! LXHasDict(dict, keep + i)) {
      (void) LXAddDict(dict, keep + i);
    }
  }

  LXTraverseDict(dict, myAction, NULL);
  
  sPrintDict(dict);
  
  for (i = 0 ; i <= 2 * 0xffff ; i++) {
    look.score = i;
    int inDict = LXHasDict(dict, &look);
    int inCheck = (check[i] != 0);
    if (inDict != inCheck) {
       printf("dictionnary error at %d inDict=%d inCheck=%d\n", i, inDict, inCheck);
       exit(1);
    }
  }
  
  LXFreeDict(dict, myStructFree);
  
  printf("test Dict OK\n");

  return 0;
}
