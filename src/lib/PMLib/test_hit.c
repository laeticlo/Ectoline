/* test Hits */
 
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "PM.h"

main() {

  int      i, nbHit;
  PMHitPtr locHit, hit[1024];
  FILE     *file;

  LXSetupIO("stdin");

  nbHit = 0;
    
  do {
    locHit = hit[nbHit++] = PMNewHit();
  } while(PMReadHit(stdin, locHit) == LX_NO_ERROR);

  nbHit--;
  
  for (i = 0 ; i < nbHit ; i++) {
    PMDebugHit(stdout, hit[i]);
  }

  file = LXOpenFile("test.asc.bak", "w");

  for (i = 0 ; i < nbHit ; i++) {
    PMWriteHit(file, hit[i]);
  }

  fclose(file);

  for (i = 0 ; i <= nbHit ; i++) {
    PMFreeHit(hit[i]);
  }

  exit(0);
}
