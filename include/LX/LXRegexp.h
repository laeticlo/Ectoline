/* -----------------------------------------------------------------------
 * $Id: LXRegexp.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: LXRegexp.h
 * @desc: pwg regexp library / include file
 * 
 * @history:
 * @+ <Gloup> : Nov 96 : first draft for PWG                 
 * @+ <Gloup> : Mar 98 : function renamed regx...                  
 * @+ <Wanou> : Jan 02 : Adapted to LX ware
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LXRegexp library<br>
 *
 * @docend:
 */

#ifndef _H_LXRegexp

#define _H_LXRegexp

#ifndef _H_LXSys
#include "LXSys.h"
#endif

/* ---------------------------------------------------- */
/* "Private" Constants (intentionally not commented)    */
/* ---------------------------------------------------- */

#define CHARBITS 0377
#define NSUBEXP  10

/* ---------------------------------------------------- */
/* "Private" Data Structures                            */
/* ---------------------------------------------------- */

typedef struct LXRegexp {
        char *startp[NSUBEXP];
        char *endp[NSUBEXP];
        char regstart;          /* Internal use only. */
        char reganch;           /* Internal use only. */
        char *regmust;          /* Internal use only. */
        int regmlen;            /* Internal use only. */
        char program[1];        /* Unwarranted chumminess with compiler. */
} LXRegExp;

/* ---------------------------------------------------- */
/* API Constants                                        */
/* ---------------------------------------------------- */

#define LX_REG_NOMATCH   -1
#define LX_REG_NOCOMPILE -2

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc:
 * Compile regular expression
 * @param exp char* expression to compile
 * @return LXRegExp* structure - to be used with LXRegExec
 */

LXRegExp *LXRegComp (char *exp);

/*
 * @doc:
 * Search regular expression in text
 * @param prog LXregexp* compiled regular expression
 * @param text char* text to search in
 * @Param bolflag Bool if TRUE consider text as new line
 *                (useful to match "^xx")
 * @return 0 failure, 1 success
 */

int LXRegExec (LXRegExp *prog, char *string, Bool bolflag);

/*
 * @doc:
 * Regexp iterative search for pattern in text.
 * this function internally keeps tracks of :
 *  -1- the compiled pattern
 *  -2- the last position found and
 * therefore it can be called repetitively to exhaust all matches
 * @param text char* text to search in
 * @param start long start index in text.
 * @param end long end index in text.
 *                     to search for the entire text, pass a negative value
 *                     as 'end'.
 * @param exp char* regular expression to search for.
 * @param jump_flag Bool TRUE : restart search at end of previous
 *                       match (if any).
 *                       FALSE : restart search at begin of previous match + 1
 * @param free_flag Bool do not search, just free internal buffers.
 *                       this is needed whenever the pattern changes.
 * @param lenfound long* get the length of the last match
 * @return start position where a match has been found between start & end
 *         LX_REG_NOMATCH (-1) if no match found
 *         LX_REG_NOCOMPILE (-2) on compilation error
 *
 * example: 
 * <pre><code>
 * // searching Pattern on whole Target
 * do {
 *    pos = LXRegSearch(Target, -1, -1, Pattern, FALSE, FALSE, &len);
 * } while (pos >= 0)
 * // free everything
 * (void) LXRegSearch(Target, -1, -1, Pattern, FALSE, TRUE, &len);
 * </code></pre>
 */

long LXRegSearch (char *text, long start, long end, 
                  char *pattern, Bool jump_flag,
                  Bool free_flag, long *lenfound);
#endif






