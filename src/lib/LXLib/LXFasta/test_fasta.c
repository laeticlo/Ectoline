/* test Fasta */

#include <stdio.h>
#include <stdlib.h>

#include "LX/LXFasta.h"

/* ----------------------------------------------- */

main(argn, argv)
	int  argn;
	char *argv[];
{
	Int32 	      nbseq;
	LXFastaSequence *seq;


	seq = LXFastaNewSequence(0);

	nbseq = 0;

	while (LXFastaReadSequence(stdin, seq)) {

	    if (! LXFastaCheckSequence(seq, TRUE))
	        LX_ERROR("fasta sequence input", LX_IO_ERROR);

        nbseq++;

	    if (nbseq % 1000 == 999) {
		  printf("\r%d", nbseq);
		  fflush(stdout);
	    }

	    LXFastaWriteSequence(stdout, seq, LX_FASTA_CHARS_PER_LINE);
	}

	LXFastaFreeSequence(seq);

	printf("total seq = %d\n", nbseq);

	exit(0);
}


	


