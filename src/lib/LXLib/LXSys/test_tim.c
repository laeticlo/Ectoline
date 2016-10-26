/* test system utilities */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <unistd.h>

#include "LX/LXSys.h"

main() {

  double now = LXCurrentTime();
  
  LXSleep(123);

  double delta = LXCurrentTime() - now;

  printf("// elapsed: %.6f\n", delta);

  LXDebug(0, "first date %d : %s\n", LXGetDebugLevel(),  LXStrCurrentTime());

  (void) LXSetDebugLevel(0);
  
  LXDebug(0, "second date %d : %s\n", LXGetDebugLevel(),  LXStrCurrentTime());
      
  exit(0);  
}
