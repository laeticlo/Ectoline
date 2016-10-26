/* test Regexp */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXRegexp.h"

main()
{
  char *pattern;
  char *text;
  long pos, len;

  pattern = NEWN(char, 16);
  text = NEWN(char, 256);

  (void) strcpy(pattern, "IS");
  (void) strcpy(text,"MISSISSIPI IS INSIDE");

  len = 0;

  do {
     pos = LXRegSearch(text, -1, -1, pattern, FALSE, FALSE, &len);
     printf("%ld %ld\n", pos, len);
  } while (pos >= 0);

  (void) LXRegSearch(text, -1, -1, pattern, FALSE, TRUE, &len);

  return 0;
}




