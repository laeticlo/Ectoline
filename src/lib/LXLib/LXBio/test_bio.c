/* test Bio */

#include <stdlib.h>
#include <string.h>

#include "LX/LXFasta.h"
#include "LX/LXBio.h"

main() {

   LXFastaSequence *seq;

   seq = LXFastaNewSequence(0);

   while (LXFastaReadSequence(stdin, seq)) {

     seq->seq = LXBioSeqTranslate(seq->seq, LX_GENETIC_CODE_UNIVL);
     seq->length = strlen(seq->seq);

     LXFastaWriteSequence(stdout, seq, 50);

   }

   (void) LXFastaFreeSequence(seq);
   
   exit(0);
}
