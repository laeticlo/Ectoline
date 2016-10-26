/* test Base64 codec */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXSys.h"
#include "LX/LXBase64.h"

#define MAXCHAR 2048

main() {

  // test LXBase64Encode/Decode in different buffers
  {
    int  src[MAXCHAR];
    char dst[MAXCHAR];
    int i, lsrc, ldst;

    lsrc = 100;
    for (i = 0 ; i < lsrc ; i++)
      src[i] = (i-50)*10;
    
    ldst = LXBase64Encode(src, lsrc*4, dst);
    
    if (ldst <= 0) {
      fprintf(stderr, "*[1]* LXBase64Encode error\n");
      exit(1);
    }
    
    lsrc = LXBase64Decode(dst, src);

    if (lsrc <= 0) {
      fprintf(stderr, "*[1]* LXBase64Decode error\n");
      exit(1);
    }
    
    for (i = 0 ; i < lsrc/4 ; i++) {
      if (src[i] != (i-50)*10) {
        fprintf(stderr, "*[1]* LXBase64Encode/Decode error\n");
        exit(3);
      }
    }
    
    printf("[1] LXBase64Encode/Decode ok\n");
  }

  // test LXBase64Encode/Decode in same buffer
  {
    int  src[MAXCHAR];
    int i, lsrc, ldst;

    lsrc = 100;
    for (i = 0 ; i < lsrc ; i++)
      src[i] = (i-50)*10;
    
    ldst = LXBase64Encode(src, lsrc*4, (char *) src);

    if (ldst <= 0) {
      fprintf(stderr, "*[2]* LXBase64Encode error\n");
      exit(1);
    }

    lsrc = LXBase64Decode((char *) src, src);

    if (lsrc <= 0) {
      fprintf(stderr, "*[2]* LXBase64Decode error\n");
      exit(1);
    }

    for (i = 0 ; i < lsrc/4 ; i++) {
      if (src[i] != (i-50)*10) {
        fprintf(stderr, "*[2]* LXBase64Encode/Decode error at %d value=%d expected %d\n",
                i, src[i], (i-50)*10);
        exit(3);
      }
    }
    
    printf("[2] LXBase64Encode/Decode ok\n");
  }

  // test external representation
  {
    char  src[MAXCHAR+1];
    int   i;
    float *f;
  
    while(fgets(src, MAXCHAR, stdin)) {
    
      char *s = src;
      int lsrc, ldst;
      Bool isLittle;

      isLittle = (s[0] == 'L');
      s += 2;
      lsrc = strlen(s);
      if (s[lsrc-1] == '\n') {
        lsrc--;
        s[lsrc] = '\000';
      }
      
      ldst = LXBase64Decode(s, s);
      
      f = (float *) s;
      
      if (isLittle) {
#ifdef LX_RT_BIG_ENDIAN
        f = LX_ENDIAN_SWAP_ADR(float, f, ldst/sizeof(float));
#endif
      }
      else {
#ifdef LX_RT_LITTLE_ENDIAN
        f = LX_ENDIAN_SWAP_ADR(float, f, ldst/sizeof(float));
#endif
      }
      
      ldst /= sizeof(float);
      
      printf("%d", ldst);
      for (i = 0 ; i < ldst ; i++) {
        printf(" %.2f", f[i]);
      }
      printf("\n");

    }
  
  }

  exit(0);
}
