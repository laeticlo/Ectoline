/* test system utilities */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <unistd.h>

#include "LX/LXSys.h"

#define N 100000000

main() {

  char *buf = NULL;
  
  printf("start: %zu\n", LXGetUsedMemory());
  
  buf = NEWN(char, N);

  printf("alloc: %zu %zu\n", LXGetUsedMemory(),  LXAllocSize(buf));

  buf = REALLOC(buf, char, 2*N);

  printf("alloc: %zu %zu\n", LXGetUsedMemory(), LXAllocSize(buf));

  buf = LXAssertAlloc(buf, 3*N);

  printf("alloc: %zu %zu\n", LXGetUsedMemory(), LXAllocSize(buf));

  FREE(buf);

  printf("end: %zu\n", LXGetUsedMemory());
     
  exit(0);  
}
