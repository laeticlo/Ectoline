/* test List */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXList.h"

/* ------------------------------------------------------------ */
/* ugly test for LXList											*/
/* ------------------------------------------------------------ */

#define LCG_A 1664525LLU
#define LCG_C 1013904223LLU
#define LCG_M 4294967296LLU

#define NB_CELLS 100000


static unsigned long seed = 1;

typedef struct {
   int indx;
   int score;
   int iamfree;
} MyStruct;

void myStructFree(void *x) {
  MyStruct *s = (MyStruct *) x;
  if (s->iamfree) {
    printf("ERROR free mystruct %d\n", s->indx);
    exit(1);
  }
  s->iamfree = 1;
}

int myStructCompar(LXCellPtr x, LXCellPtr y) {
  MyStruct * xx = (MyStruct *) x->body;
  MyStruct * yy = (MyStruct *) y->body;
  return (xx->score - yy->score);
}

int myStructFind(LXCellPtr cell) {
  MyStruct *xx = (MyStruct *) cell->body;
  return (xx->score % 2);
}

int myStructFindDummy(LXCellPtr cell) {
  MyStruct *xx = (MyStruct *) cell->body;
  return (xx->indx < 0);
}


static int pseudoRand(int max) {
  seed = (LCG_A * seed + LCG_C) % LCG_M;
  return (seed % max);
}

int main() {

  int i;
  MyStruct keep[NB_CELLS];
  MyStruct dummy = {-1, -1, 0};
  
  LXCellPtr root = NULL;
  LXCellPtr last = NULL;
  
  for (i = 0 ; i < NB_CELLS ; i++) {
      int k = pseudoRand(1000);
      keep[i].indx = i;
      keep[i].score = k;
      keep[i].iamfree = 0;
      
      last = LXAddCell(last ? &last : &root, LXNewCell(keep + i));
  }

  printf("N = %zu root=0x%lx\n", LXNumberOfCells(root), (unsigned long) root);
  
  root = LXInsertCellBefore(root, LXNewCell(&dummy));
  
  printf("N = %zu root=0x%lx\n", LXNumberOfCells(root), (unsigned long) root);
  
  (void) LXInsertCellAfter(root, LXNewCell(&dummy));
  
  printf("N = %zu root=0x%lx\n", LXNumberOfCells(root), (unsigned long) root);

  LXCellPtr copy = LXCopyList(root, NULL);

  printf("N = %zu copy=0x%lx\n", LXNumberOfCells(copy), (unsigned long) copy);

  LXFreeCells(&copy, NULL);

  printf("N = %zu copy=0x%lx\n", LXNumberOfCells(copy), (unsigned long) copy);

  LXCellPtr fnd = LXFindCell(root, LX_SEARCH_DOWNSTREAM, myStructFind);
  
  printf("N = %d found=0x%lx\n", (fnd ? 1 : 0), (unsigned long) fnd);

  LXCellPtr lst = LXFindCells(root, LX_SEARCH_DOWNSTREAM, myStructFind);

  printf("N = %zu find=0x%lx\n", LXNumberOfCells(lst), (unsigned long) lst);

  LXFreeCells(&lst, NULL);
  
  printf("N = %zu root=0x%lx\n", LXNumberOfCells(root), (unsigned long) root);
  
  while (fnd = LXFindCell(root, LX_SEARCH_DOWNSTREAM, myStructFindDummy)) {
    if (fnd == root)
        root = fnd->next;
    LXDisconnectCell(fnd);
    LXFreeCell(fnd, NULL);
  }

  printf("N = %zu root=0x%lx\n", LXNumberOfCells(root), (unsigned long) root);

  LXFreeSelectedCells(&root, myStructFind, myStructFree);

  printf("N = %zu root=0x%lx\n", LXNumberOfCells(root), (unsigned long) root);

  LXFreeSelectedCells(&root, myStructFind, myStructFree);

  printf("N = %zu root=0x%lx\n", LXNumberOfCells(root), (unsigned long) root);

  for (i = 0 ; i < NB_CELLS ; i++) {
    if ((keep[i].score % 2) != keep[i].iamfree) {
        printf("free error\n");
        exit(1);
    }
  }

  printf("free ok\n");

  LXSortCells(&root, myStructCompar);

  printf("N = %zu root=0x%lx\n", LXNumberOfCells(root), (unsigned long) root);

  int previous = -1;
  LXCellPtr e;
  for (e = root ; e ; e = e->next) {
    MyStruct *s = (MyStruct *) e->body;
    if (s->score < previous) {
      printf("sort error\n");
      exit(1);
    }
    if (s->score != previous)
      previous = s->score;
    if (s->iamfree) {
      printf("free error\n");
      exit(1);
    }
  }

  printf("sort ok\n");

  LXFreeCells(&root, myStructFree);

  printf("N = %zu root=0x%lx\n", LXNumberOfCells(root), (unsigned long) root);

  for (i = 0 ; i < NB_CELLS ; i++) {
    if (! keep[i].iamfree) {
      printf("free error at %d\n", i);
      exit(1);
    }
  }  
  
  return 0;
}
