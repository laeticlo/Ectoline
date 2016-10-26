/* ---------------------------------------------------------------- */
/* @file: regsearch.c                                               */
/* @desc: pwg regexp library / reg. expr. search                    */
/* ---------------------------------------------------------------- */
/*                                                                  */
/* @history:                                                        */
/* @+       <Gloup> : Sep 92 : first draft for Smarties             */
/* @+       <Gloup> : Mar 97 : moved to PWG                         */
/*                                                                  */
/* ---------------------------------------------------------------- */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "LX/LXRegexp.h"

/* -------------------------------------------------------------------- */
/* performs a simple regular regexp search                              */
/* returns offset (start counted) if found (-1 else)                    */
/* options (Boolean) are :                                              */
/* jump_flag : flags for next search :                                  */
/*             TRUE : restart search at end of previous match (if any)  */
/*             FALSE : restart jump at begin of previous match + 1      */
/* free_flag : do not search, just free any allocated buffer            */
/*             (needed when pattern changes) (returns 0)                */
/* -------------------------------------------------------------------- */
/* Note : -1 always means error                                         */
/* -------------------------------------------------------------------- */
/* Note : to search for the entire sequence, pass negative values       */
/*        as 'start' and 'end'                                          */
/* -------------------------------------------------------------------- */
/* example : searching Pattern on whole Target                          */
/* do {                                                                 */
/*    pos = regsearch(Target, -1, -1, Pattern, FALSE, FALSE, &len);     */
/*    ...                                                               */
/* while (pos >= 0);                                                    */
/* (void) regsearch(Target, -1, -1, Pattern, FALSE, TRUE, &len);        */
/* -------------------------------------------------------------------- */

long LXRegSearch(char *seq, long start, long end, char *pattern,
                 Bool jump_flag, Bool free_flag, long *lenfound)
{
        int     starting, okSearch;
        char    *seqdeb, *seqfin, *target, csave;
        
                                            /* -------------------- */
        static  LXRegExp *regexp = NULL;    /* keep these as static */
        static long lenseq = 0;             /* for next call        */
                                            /* -------------------- */

                                            /* -------------------- */
        if (free_flag) {                    /* just free memory     */
                                            /* -------------------- */
            if (regexp) 
                FREE(regexp);       /* regexp did a MALLOC  */
            regexp = NULL;
            lenseq = 0;
            return 0;
        }
        
                                                    /* -------------------- */
        if ((starting = (regexp == NULL)) != 0) {   /* compile regexp       */
                                                    /* -------------------- */
                
            if (! (regexp = LXRegComp(pattern)))
                return LX_REG_NOCOMPILE;

            lenseq = strlen(seq);
        }

                                            /* -------------------- */
                                            /* prepare target       */
                                            /* -------------------- */
        if (start < 0) start = 0;
        if (end   < 0) end   = lenseq - 1;
        
        if ((start > end) || (end >= lenseq))
            return -1;

        seqdeb  = seq + start;
        seqfin  = seq + end + 1;            /* 1 char after limit   */

                                            /* -------------------- */
        if (starting) {                     /*  first search        */
          target = seqdeb;                  /* -------------------- */
        }
                                            /* -------------------- */
        else {                              /* find next            */
                                            /* -------------------- */
            if (jump_flag)
                target = regexp->endp[0];
            else
                target = regexp->startp[0] + 1;

            if (   (target < seqdeb)
                || (target >= seqfin))      /* bad target           */
                return -1;
        }
                                            /* -------------------- */
                                            /* performs search      */
                                            /* -------------------- */
        csave     = *seqfin;
        *seqfin   = '\000';
        *lenfound = 0;

        okSearch = LXRegExec(regexp, target, TRUE);

        *seqfin = csave;

        if (! okSearch)                     /* not found            */
            return LX_REG_NOMATCH;

        if (regexp->startp[0] >= seqfin)    /* too far              */
            return LX_REG_NOMATCH;                      
                                            /* -------------------- */
                                            /* return results       */
                                            /* -------------------- */

        *lenfound =   (unsigned long) (regexp->endp[0]) 
                    - (unsigned long) (regexp->startp[0]);
        
        return ((unsigned long) (regexp->startp[0]) - (unsigned long) seqdeb);
}








