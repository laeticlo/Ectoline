/* test binary read/write */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXSys.h"

main() {

  Int32 ifrom, ito;
  float ffrom, fto;
  char  chrFrom[32], chrTo[32];
  char  strFrom[32], strTo[32];
  char  fname[BUFSIZ];
  
  FILE *stream;

  // test endianess
  
  Bool little = LXIsLittleEndian();
  
#ifdef LX_RT_LITTLE_ENDIAN
  if (! little) {
#else
  if (little) {
#endif
     LX_ERROR("Inconsistant endianess declaration", LX_ARCH_ERROR);
  }
  (void) printf("@ this is a %s endian machine\n", little ? "little" : "big");

  // test binary write/read

  stream = LXOpenFile("test_bin.out", "wb");

  ifrom = -123456;
  ffrom = 1.2345;
  (void) strcpy(chrFrom, "123456");
  (void) strcpy(strFrom, "6543210");
  
  (void) LXBinWriteInt32(stream, ifrom);
  (void) LXBinWriteFloat(stream, ffrom);
  (void) LXBinWriteChars(stream, chrFrom, 32);
  (void) LXBinWriteStr(stream, strFrom);
  
  fclose(stream);

  stream = LXOpenFile("test_bin.out", "rb");
  
  (void) LXBinReadInt32(stream, &ito);
  (void) LXBinReadFloat(stream, &fto);
  (void) LXBinReadChars(stream, chrTo, 32);
  (void) LXBinReadStr(stream, strTo);

  fclose(stream);
   
  (void) printf("-> i=%d f=%.4f c=%s s=%s\n", ifrom, ffrom, chrFrom, strFrom);
  (void) printf("<- i=%d f=%.4f c=%s s=%s\n", ito, fto, chrTo, strTo);
  
  if ((ifrom != ito) || (ffrom != fto) || strcmp(chrFrom, chrTo) || strcmp(strFrom, strTo)) {
    printf ("* Error *\n");
    exit (1);
  }
  
  // access test
  
  (void) strcpy(fname, "test_bin.c");
  printf("access %s : %d\n", fname, LXAccess(fname, "r"));

  (void) strcpy(fname, "test_bin.out");
  printf("access %s : %d\n", fname, LXAccess(fname, "r"));

  (void) strcpy(fname, "test_bin.nothere");
  printf("access %s : %d\n", fname, LXAccess(fname, "r"));
    
  exit(0);  
}
