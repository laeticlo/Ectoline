/* -----------------------------------------------------------------------
 * $Id: LXString.c 1782 2013-02-23 00:11:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXString.c
 * @desc: some strings utility functions
 * 
 * @history:
 * @+ <Hdp>   : Jan 94 : first version for PWG
 * @+ <Gloup> : Feb 94 : cleaned & speedup
 * @+ <Wanou> : Jan 02 : adapted to LX_Ware 
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LX string library<br>
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXString.h"

#define LINE_FEED   '\n'
#define CARR_RETURN '\r'

/* ---------------------------- */

String LXNewStr(size_t size)
{
    size = MAX(size, 0);
    
    String str = NEWN(char, size+1);
    
    if (str) str[size] = '\000';
      
    return str;
}

/* ---------------------------- */

String LXStr(const char *src)
{
    return LXStrcpy(NULL, src);
}

/* ---------------------------- */

String LXStrcpy(String dst, const char *src)
{
    size_t len = (src ? strlen(src) : 0);
    
    if (! dst) dst = LXNewStr(len);
    
    if ((dst = LXAssertAlloc(dst, len+1)) != NULL)
      (void) strcpy(dst, src ? src : "");
      
    return dst;
}

/* ---------------------------- */

String LXStrncpy(String dst, const char *src, size_t size)
{
    if (! dst) dst = LXNewStr(size);
    
    if ((dst = LXAssertAlloc(dst, size+1)) != NULL) {
      (void) strncpy(dst, src ? src : "", size);
      dst[size] = '\000';
    }

    return dst;
}

/* ---------------------------- */

String LXStrcat(String dst, const char *src)
{
    size_t len = (src ? strlen(src) : 0);
    
    if (! dst) dst = LXNewStr(len);

    len += strlen(dst);

    if ((dst = LXAssertAlloc(dst, len+1)) != NULL) {
        (void) strcat(dst, src);
    }

    return dst;
}

/* ---------------------------- */

char *LXEraseChar(char *str, char c)
{
    char *s, *se;

    if (! str)
        return str;
        
    for (se = s = str ; *s ; s++)
        if ((*se = *s) != c)
            se++;

    *se = '\000';

    return str;
}

/* ---------------------------- */

char *LXReplaceChar(char *str, char cfrom, char cto)
{
    char *s;

    if (! str)
        return str;
        
    for (s = str ; *s ; s++)
        if (*s == cfrom)
            *s = cto;

    return str;
}

/* ---------------------------- */

char *LXTrimTrailing(char *str)
{
    char *s;
    
    if (! str)
       return str;
            
    s = str + strlen(str);

    for (--s ; (s >= str) && isspace(*s) ; s--)
        /* nop */ ; 

    *++s = '\000';

    return str;     
}

/* ---------------------------- */

char *LXTrimLeading(char *str)
{
    char *sb, *sn;
    
    if (! (str && isspace(*str)))
        return str;
            
    for (sb = sn = str ; isspace(*sn) ; sn++) 
        /* nop */ ;

    while (*sn)
        *sb++ = *sn++;

    *sb = '\000';
    
    return str;
}

/* ---------------------------- */

char *LXPadRight(char *str, size_t size, char padchar)
{
    size_t len;
    char *s;

    if (! (str && (len = strlen(str)) < size))      
        return str;

    s = str + len;

    size -= len;
    
    while (size--)
        *s++ = padchar;
    
    *s = '\000';

    return str;
}

/* ---------------------------- */

char *LXPadLeft(char *str, size_t size, char padchar)
{
    size_t len;
    char *s, *t;

    if (! (str && (len = strlen(str)) < size))      
        return str;

    s = str + len;
    *(t = str + size) = '\000';

    while (len--)
        *--t = *--s;

    while (t > str)
        *--t = padchar;
            
    return str;
}

/* ---------------------------- */

char *LXDropStr(char *str, size_t start, size_t nchars)
{
    size_t len;
    char *sb, *sn;

    if (! (str && (len = strlen(str)) > start))
        return str;

    if (len < (start + nchars))
        nchars = len - start;

    sb = str + start;
    sn = sb + nchars;
    
    while (*sn)
        *sb++ = *sn++;

    *sb = '\000';
    
    return str;
}

/* ---------------------------- */

String LXInsertStr(String dst, char *src, size_t pos)
{
    size_t srclen, dstlen;
    char *d, *t;
    
    srclen = src ? strlen(src) : 0;
    dstlen = dst ? strlen(dst) : 0;

    if ((srclen == 0) || (dstlen < pos))
      return dst;

    String tmp = LXAssertAlloc(dst, srclen + dstlen + 1);
    if (! tmp)
      return dst;
      
    dst = tmp;

    d = dst + dstlen;
    t = d + srclen;
    
                    
    *t = '\000';

    dstlen -= pos;
    
    while (dstlen--)
        *--t = *--d;

    t = src;

    while(srclen--)
        *d++ = *t++;

    return dst;
}

/* ---------------------------- */

char *LXExtractToMark(char *dst, char *src, size_t start, char markchar)
{
    size_t len;
    char *d;

    if (! (src && dst))
        return dst;

    if ((len = strlen(src)) < start)
        start = len;

    src += start;

    for (d = dst ; *src && (*src != markchar) ; src++, d++)
        *d = *src;

    *d = '\000';

    return dst;
}

/* ---------------------------- */

char *LXExtractStr(char *dst, char *src, size_t from, size_t to)
{
    size_t len;
    char *d, *end;

    if (! (src && dst))
        return dst;

    len = strlen(src);
    
    if (len < from)
        from = len;

    if (len < to)
        to = len;
        
    end = src + to;
    
    src += from;

    for (d = dst ; *src && (src <= end) ; src++, d++) 
        *d = *src;

    *d = '\000';

    return dst;
}

/* ---------------------------- */

char *LXReverseStr(char *str)
{
    char *sb, *se, c;

    if (! str)
        return str;
        
    sb = str;
    se = str + strlen(str) - 1;

    while(sb <= se) {
       c    = *sb;
      *sb++ = *se;
      *se-- = c;
    }

    return str;
}

/* ---------------------------- */

#define IS_LOWER(c) (((c) >= 'a') && ((c) <= 'z'))
#define TO_UPPER(c) ((c) - 'a' + 'A')

char *LXUpperStr(char *str)
{
    char *s;

    if (! str)
        return str;

    for (s = str ; *s ; s++)
        if (IS_LOWER(*s))
            *s = TO_UPPER(*s);

    return str;    
}

#undef IS_LOWER
#undef TO_UPPER

/* ---------------------------- */

#define IS_UPPER(c) (((c) >= 'A') && ((c) <= 'Z'))
#define TO_LOWER(c) ((c) - 'A' + 'a')

char *LXLowerStr(char *str)
{
    char *s;

    if (! str)
        return str;

    for (s = str ; *s ; s++)
        if (IS_UPPER(*s))
            *s = TO_LOWER(*s);
        
    return str;    
}

#undef IS_UPPER
#undef TO_LOWER


/* -------------------------------------------- */

long LXReadStr(FILE *stream, String *buf) {

  static char sBuffer[BUFSIZ+1];    /* in <stdio.h>   */

  if (! *buf) *buf = LXNewStr(0);
  
  **buf = '\000';
  
  long readlen = 0;
  
  char *sbuf = NULL;
  
  Bool more = TRUE;

  while (more) {
  
    sbuf = fgets(sBuffer, sizeof(sBuffer), stream);

    if (! sbuf) break; // EOF reached

    long len = MAX(1, strlen(sbuf));

    more = (sbuf[len-1] != LINE_FEED);
    
    if (sbuf[len-1] == LINE_FEED)   sbuf[--len] = '\000';
    if (sbuf[len-1] == CARR_RETURN) sbuf[--len] = '\000';

    readlen += len;

    if (! (*buf = LXStrcat(*buf, sbuf))) break;
  }

  return (readlen ? readlen : (sbuf ? (long) strlen(sbuf) : -1));
}

