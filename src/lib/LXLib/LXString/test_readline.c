/* test LineBuf */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXString.h"

/* ----------------------------------------------- */

main(argn, argv)
	int  argn;
	char *argv[];
{
    long nline = 0;
    String buf = NULL;

    while (LXReadStr(stdin, &buf) >= 0) {
      printf("%ld %zu\n", ++nline, strlen(buf));
    }
    
	printf("total line = %ld\n", nline);

    FREE(buf);
    
	exit(0);
}


	


