/* test LXPat */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXSys.h"
#include "LX/LXStkp.h"
#include "LX/LXPat.h"

main()
{
  Int32 i, pos, len;
  LXPattern pat;
  LXStackpPtr stk;
  char *seq;
  
  stk = LXNewStackp(LX_STKP_MIN_SIZE);
  
  strcpy(pat.cPat,"BBCB");
  pat.patLen = strlen(pat.cPat);
  pat.maxErr = 0;

  LXPatEncodePattern(&pat, ALPHA_TYPE);

  LXPatCreateS(&pat);

  // LXPatPrintDebugPattern(&pat);

  seq = NEWN(char, BUFSIZ);

  (void) strcpy(seq,"BBCBAABBCBABCBCBAA");

  len = strlen(seq);

  /* 
   * no error
   */

  (void) LXPatEncodeSequence(seq);
  
  pat.maxErr = 0;
  
  LXPatManber(seq, &pat, 0, len, stk);
  
  (void) LXPatDecodeSequence(seq);
  
  while (LXPoppOut(stk, (Ptr*) &pos)) {
    printf("[0] %d %4.4s\n", pos, seq + pos);
  }
  
  /* 
   * 1 substitution
   */
   
  (void) LXPatEncodeSequence(seq);
  
  pat.maxErr = 1;
  pat.hasIndel = 0;
  
  LXPatManber(seq, &pat, 0, len, stk);
  
  (void) LXPatDecodeSequence(seq);
  
  while (LXPoppOut(stk, (Ptr*) &pos)) {
    printf("[s] %d %4.4s\n", pos, seq + pos);
  }

  exit (0);
}







