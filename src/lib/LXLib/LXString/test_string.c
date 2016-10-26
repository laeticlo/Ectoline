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
    String s1 = NULL, s2 = NULL;
    
    const char *c = "abcdefg";
    
    int n = 1000;
    while (n--) {
      s1 = LXStrcat(s1, c);
    }
    
    printf("%zu %zu\n", strlen(s1), LXAllocSize(s1));
    
    s2 = LXStrcpy(s2, s1);

    printf("%zu %zu\n", strlen(s2), LXAllocSize(s2));
    
	exit(0);
}
