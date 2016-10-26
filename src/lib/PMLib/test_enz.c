/* test Enzymes */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "PM.h"

main() {

  PMEnzyme enz;

  LXSetupIO("stdin");

  while (PMReadEnzyme(stdin, &enz) == LX_NO_ERROR)
    PMDebugEnzyme(stdout, &enz);

  exit(0);
}
