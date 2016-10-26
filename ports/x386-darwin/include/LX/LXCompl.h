/* -----------------------------------------------------------------------
 * $Id: LXCompl.h 1038 2010-04-18 21:12:30Z viari $
 * -----------------------------------------------------------------------
 * @file: LXCompl.h
 * @desc: job completion notifier
 * 
 * @history:
 * @+ <Gloup> : Jan 96 : first version for PWG 
 * @+ <Gloup> : Jan 10 : random generators added 
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * job completion notifier<br>
 *
 * @docend:
 */

#ifndef _H_LXCompl

#define _H_LXCompl

#include "LXSys.h"

/* ---------------------------------------------------- */
/* Constants  											*/
/* ---------------------------------------------------- */

/*
 * @doc: todo
 */
 
/* ---------------------------------------------------- */
/* Types    											*/
/* ---------------------------------------------------- */

/*
 * @doc: todo
 */
typedef enum {
    LX_COMPLETER_AUTO = 0,
    LX_COMPLETER_ANSI,
    LX_COMPLETER_SIMPLE,
    LX_COMPLETER_TEXT,
    LX_COMPLETER_NONE
} LXComplMode;

/*
 * @doc: todo
 * the actual implementation is hidden, please use API
 */
typedef struct LXCompl_Struct *LXComplPtr;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: free completer
 * @param comp LXComplPtr completer to free
 * @return NULL
 */

LXComplPtr LXFreeCompleter(LXComplPtr comp);

/*
 * @doc: make new completer
 * @param mode LXComplMode : LX_COMPLETER_X (X = AUTO | ANSI SIMPLE | TEXT NONE )
 * @param name char* name
 * @param namx size_t max completion value
 * @param units char* units
 * @param deltamax double min delta time step (in ms)
 * @param userinfo *char[] optional user information : end array with NULL
 * @return new completer
 */

LXComplPtr LXNewCompleter(LXComplMode mode, char *name, size_t nmax,
                          char *units, double deltamax, char *userinfo[]);
                            
/*
 * @doc: next completion round. Should be called regularly to update completer.
 * @param comp LXComplPtr completer to update
 * @param curr size_t current completion value
 * @return LX_NO_ERROR on success
 */

int LXDoCompleter(LXComplPtr comp, size_t curr);

/*
 * @doc: you should call this when job is finished
 * @param comp LXComplPtr completer to finish
 * @return LX_NO_ERROR on success
 */

int LXEndCompleter(LXComplPtr comp);

/*
 * @doc: reset completer for another run
 * @param comp LXComplPtr completer to finish
 * @return LX_NO_ERROR on success
 */

int LXResetCompleter(LXComplPtr comp);

#endif



