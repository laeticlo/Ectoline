/* -----------------------------------------------------------------------
 * $Id: LXString.h 1780 2013-02-18 00:50:17Z viari $
 * -----------------------------------------------------------------------
 * @file: LXString.h
 * @desc: generic string library/ include file
 * 
 * @history:
 * @+ <Gloup> : Jan 96 : first version for PWG 
 * @+ <Wanou> : Jan 02 : adapted to LX_Ware 
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LX string library / include file<br>
 *
 * Whenever specified in the following functions, a 'String' argument
 * is equivalent to char* (see LXTypes.h).
 * The only difference is that a String knows about its allocated
 * size and can therefore resize if needed. You see String as a
 * dynamic char*.
 * You should allocate and free Strings thru NEWN, FREE
 * or similar macros in LXMacros.h
 *
 * @docend:
 */
        
#ifndef _H_LXString

#define _H_LXString

#include <string.h>

#include "LXSys.h"

/* ---------------------------------------------------- */
/* Prototypes of library functions                      */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * make a new empty string of size+1 bytes.
 * this equivalent to NEWN(char, size+1) except that:
 *   - if size < 0 it is set to a minimal size of 1
 *   - the resulting String is null-terminated
 * @param size_t size in bytes
 * @return String
 */
String LXNewStr(size_t size);

/*
 * @doc: 
 * make a new string from char *.
 * @param char *src
 * @return String
 */
String LXStr(const char *src);

/*
 * @doc: 
 * copy src into dst, sizing dst as needed.
 * if dst == NULL then it is allocated.
 * note: make sure to reassign dst upon return<br>
 * <pre><code>
 * dst = LXStrcpy(dst, src);
 * </code></pre>
 * @param dst destination string
 * @param src source string
 * @return dst String
 */                      
String LXStrcpy(String dst, const char *src);

/*
 * @doc: 
 * copy at most size bytes from src into dst, sizing dst as needed.
 * If src is less than size characters long, the remainder of dst is
 * filled with `\000' characters.
 * if dst == NULL then it is allocated.
 * note: make sure to reassign dst upon return:
 * <pre><code>
 * dst = LXStrncpy(dst, src);
 * </code></pre>
 * upon return dst is always null-terminated.
 * @param dst destination string
 * @param src source string
 * @param size max number of bytes to copy
 * @return dst String 
 */                      
String LXStrncpy(String dst, const char *src, size_t size);

/*
 * @doc: 
 * catenate src into dst, sizing dst as needed.
 * if dst == NULL then it is allocated.
 * note: make sure to reassign dst upon return:
 * <pre><code>
 * dst = LXStrcpy(dst, src);
 * </code></pre>
 * @param dst destination string
 * @param src source string
 * @return dst String 
 */                      
String LXStrcat(String dst, const char *src);

/*
 * @doc: 
 * Erase fisrt occurence of a char in a string
 * @param str char* string to parse
 * @param c char char to erase
 * @return modified str
 */
char *LXEraseChar (char *str, char c);

/*
 * @doc: 
 * Replace in a string all occurences of a given char by an over one
 * @param str char* string to parse
 * @param cfrom char char to be replaced
 * @param cto char char to put instead of cfrom
 * @return modified str
 */
char *LXReplaceChar (char *str, char cfrom, char cto);

/*
 * @doc: 
 * Trim trailing spaces from 'str'
 * @param str char* string to parse
 * @return modified str
 */
char *LXTrimTrailing (char *str);

/*
 * @doc: 
 * Trim leading spaces from 'str'
 * @param str char* string to parse
 * @return modified str
 */
char *LXTrimLeading (char *str);

/*
 * @doc:
 * Pad 'str' with char 'padchar' on right to built a string of
 * length 'size'
 * @param str char*
 * @param size size_t
 * @param char padchar
 * @return modified str
 */
char *LXPadRight (char *str, size_t size, char padchar);

/*
 * @doc:
 * Pad 'str' with char 'padchar' on right to built a string of
 * length 'size'
 * @param str char*
 * @param size size_t
 * @param char padchar
 * @return modified str
 */
char *LXPadLeft (char *str, size_t size, char padchar);

/*
 * @doc:
 * Delete 'nchars' chars from 'str', starting at 'start'
 * @param str char*
 * @param start size_t
 * @param nchars size_t                                                       
 * @return modified str
 */
char *LXDropStr (char *str, size_t start, size_t nchars);

/*
 * @doc:
 * Insert 'src' into 'dst', at position 'pos', resizing as necessary.
 * pos should be between 0 and strlen(dst)
 * note: make sure to reassign dst upon return:
 * <pre><code>
 * dst = LXInsertStr(dst, src, 0);
 * </code></pre>
 * @param dst char*
 * @param src char*
 * @param pos size_t
 * @return modified dst
 */
String LXInsertStr (char *dst, char *src, size_t pos);

/*
 * @doc:
 * Extract substring from string 'src', starting at 'from' up to 'to'
 * included (or end of string)
 * @param dst char*
 * @param src char*
 * @param from size_t
 * @param to size_t
 * @result modified dst 
 */
char *LXExtractStr (char *dst, char *src, size_t from, size_t to);

/*
 * @doc:
 * Put into 'dst' the substring from 'src' starting at 'start' up
 * to the char 'markchar' (or end of string)
 * @param dst char*
 * @param src char*
 * @param start size_t
 * @param markchar char
 * @result modified dst
 */ 
char *LXExtractToMark (char *dst, char *src, size_t start, char markchar);

/*
 * @doc:
 * uppercase string 'str' 
 * @param str char*
 * @result modified str
 */
char *LXUpperStr (char *str);

/*
 * @doc:
 * lowercase string 'str' 
 * @param str char*
 * @result modified str
 */
char *LXLowerStr (char *str);

/*
 * @doc:
 * reverse string 'str' 
 * @param str char*
 * @result modified str
 */
char *LXReverseStr (char *str);

/*
 * @doc:
 * Read arbitrary long line from stream into String, resizing as necessary.
 * if buf == NULL then it is allocated.
 * note: make sure to reassign buf upon return:
 * @param stream FILE* to read in
 * @param buf String* to fill
 * @return number of char read (-1 if EOF reached)
 * @sample
 * 
 * <b>Example:</b>
 * <pre><code>
 * String buf = NULL;
 * while (LXReadStr(streamin, &buf) >= 0) {
 *      myProcessLine(buf);
 * }
 * </code></pre>
 */
long LXReadStr (FILE *stream, String *buf);

#endif
