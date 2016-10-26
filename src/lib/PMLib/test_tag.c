/* test Tags */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "PM.h"

main() {

  int i;
  PMTag tag;
  FILE *ascfile, *binfile;

  /* write ascii/binary tag */

  printf("// ASCII/BIN\n");
  
  ascfile = LXOpenFile("test_tag.asc","w");
  binfile = LXOpenFile("test_tag.bin","wb");
  
  while (PMReadTag(stdin, &tag) == LX_NO_ERROR) {
    PMDebugTag(stdout, &tag);
    PMWriteTag(ascfile, &tag);
    PMBinWriteTag(binfile, &tag);
  }

  fclose(ascfile);
  fclose(binfile);

  /* read ascii tag */

  printf("// ASCII\n");
  
  ascfile = LXOpenFile("test_tag.asc","r");

  while (PMReadTag(ascfile, &tag) == LX_NO_ERROR) {
    PMDebugTag(stdout, &tag);
  }
  fclose(ascfile);

  printf("// BINARY\n");
    
  /* read binary tag */
  
  binfile = LXOpenFile("test_tag.bin","rb");

  while (PMBinReadTag(binfile, &tag) > 0) {
    PMDebugTag(stdout, &tag);
  }
  fclose(binfile);
  
  exit(0);
}
