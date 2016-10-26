/* test system utilities */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <unistd.h>

#include "LX/LXSys.h"


main() {

  LXTmpFile tmpfile;
  int err;
  
  LXOpenTmpFile(&tmpfile);

  fprintf(stderr, ">%s<\n", tmpfile.fileName);
  
  fprintf(tmpfile.stream, "output temporary");

  err = LXCloseTmpFile(&tmpfile);
  
  if (err != LX_NO_ERROR)
    IO_ERROR();

  exit(0);  
}
