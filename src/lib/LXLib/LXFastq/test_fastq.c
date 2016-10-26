/* test Fasta */

#include <stdio.h>
#include <stdlib.h>

#include "LX/LXFastq.h"

/* ----------------------------------------------- */

main(argn, argv)
	int  argn;
	char *argv[];
{
	Int32 	      nbseq;
	LXFastqSequence *seq;
	LXFastqEncoding code = LX_FASTQ_SANGER;
	
	if (argn > 1) {
	  if (! strcmp(argv[1], "sanger"))
	    code = LX_FASTQ_SANGER;
	  if (! strcmp(argv[1], "illumina"))
	    code = LX_FASTQ_ILLUMINA;
	  if (! strcmp(argv[1], "solexa"))
	    code = LX_FASTQ_SOLEXA;
	}
	
	seq = LXFastqNewSequence(0);

	nbseq = 0;

	while (LXFastqReadSequence(stdin, seq, code)) {

        nbseq++;

	    if (nbseq % 1000 == 999) {
		  printf("\r%d", nbseq);
		  fflush(stdout);
	    }

	    LXFastqWriteSequence(stdout, seq);

	}

	LXFastqFreeSequence(seq);

	exit(0);
}


	


