/* test Dynamic Buffer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXArray.h"

#define N 10
#define M 15

int compare(const void *p1, const void *p2) {
    return *((int *) p1) - *((int *) p2);
}


main() {

  LXArray *buf = LXNewArray(sizeof(int), 0);
  
  int v1[N];
  int i;
  
  for (i = 0; i < N ; i++)
    v1[i] = i;
    
  LXAppendArray(buf, v1, N);
  
  for (i = 0; i < N ; i++)
    v1[i] = 2 * i;
  
  LXAppendArray(buf, v1, N);

  LXSetArrayCapacity(buf, M);

  // printout

  LXSortArray(buf, compare);
  
  int *vv = (int *) buf->data;
  
  for (i = 0 ; i < buf->nbelt ; i++) {
    printf("%d %d\n", i, vv[i]);
  }

  (void) LXFreeArray(buf);
  
  exit(0);  
}
