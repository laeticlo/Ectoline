/* test AminoAcids */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "PM.h"

main() {

  int i;

  PMAminoAlphabet alpha;
  PMAminoAcid *aa;

  LXSetupIO("stdin");

  (void) PMReadAminoAlphabet(stdin, &alpha);

  PMDebugAminoAlphabet(stdout, &alpha);
  
  (void) printf("complete: %s\n", PMGetFullAlphabet(&alpha));
  (void) printf("normal  : %s\n", PMGetNormalAlphabet(&alpha));

  aa = PMGetAminoAcid(&alpha, 'C');
  
  PMDebugAminoAcid(stdout, aa);
  
  for (i = 0 ; i < aa->nbMod ; i++) {
    PMAminoAcid *amod = PMGetAminoAcid(&alpha, aa->modIndx[i]);
    PMDebugAminoAcid(stdout, amod);
  }
  
  
  exit(0);
}
