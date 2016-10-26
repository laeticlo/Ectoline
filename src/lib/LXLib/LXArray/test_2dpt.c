/* test Dynamic Buffer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXArray.h"

#define N 10

typedef struct {
    int x, y;
} Point;

int compare(const void *p1, const void *p2) {
    return ((Point *) p2)->x - ((Point *) p1)->x;
}

main() {

  LXArray *buf = LXNewArray(sizeof(Point), 0);
  
  int i;
  
  for (i = 0; i < N ; i++) {
    Point p = {i, 2*i};
    LXAppendArray(buf, &p, 1);
  }
  
  // printout

  LXSortArray(buf, compare);
  
  Point *pt = (Point *) buf->data;
  
  for (i = 0 ; i < buf->nbelt ; i++) {
    printf("%d : %d %d\n", i, pt[i].x, pt[i].y);
  }

  (void) LXFreeArray(buf);
  
  exit(0);  
}
