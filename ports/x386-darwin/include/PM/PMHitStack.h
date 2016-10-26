/* -----------------------------------------------------------------------
 * $Id: PMHitStack.h 1935 2013-07-25 12:38:55Z viari $
 * -----------------------------------------------------------------------
 * @file: PMHitStack.h
 * @desc: stack of hits library
 *
 * @history:
 * @+ <Wanou>  : Jan 03 : first version
 * @+ <Gloup>  : Dec 05 : major revision
 * @+ <Gloup>  : Feb 06 : code cleanup
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * stack of hits management library<br>
 * specialization of LXStackp
 *
 * @docend:
 */

#ifndef _H_PMHitStack
#define _H_PMHitStack

#include "LX.h"

#include "PMHit.h"

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/* ---------------------------------------------------- */
/* Types & Data Structures                              */
/* ---------------------------------------------------- */

/*
 * @doc: sort type 
 * - sortHitsByPosMn : selector for sorting hits according their posMn position
 * - sortHitsByPosSn : selector for sorting hits according their posSn position
 * - sortHitsByScore : selector for sorting hits according their score
 */
typedef enum {
  sortHitsByPosMn,
  sortHitsByPosSn,
  sortHitsByScore
} PMHitSortType;

/* 
 * @doc: HitStack types (just aliases)
 */

typedef LXStackp     PMHitStack; 
typedef LXStackpPtr  PMHitStackPtr; 
typedef LXStackpHdle PMHitStackHdle; 

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 *@doc 
 *Create a new (empty) PMHitStack
 *@return new PMHitStack*
 */

PMHitStack *PMNewHitStack(void);

/*
 *@doc 
 *Free a PMHitStack (freeing hits too)
 *@param PMHitStack *stk (PMHitStack to free) 
 *@return NULL;
 */

PMHitStack *PMFreeHitStack(PMHitStack *stkp);

/*
 *@doc 
 *Sort a PMHitStack according to hit attributes
 *@param PMHitStack *stk (PMHitStack to sort) 
 *@param PMHitSortType sorting criterion
 */

void PMSortHitStack(PMHitStack *stk, PMHitSortType sortBy);

/*
 *@doc 
 *Read PMHitStack from file and Push hits in a PMHitStack
 *@param FILE *streamin (file containing hits)
 *@param PMHitStack *stkp (PMHitStack to fill with hits)
 *@param skipMark (skip % marks if TRUE)
 *@return error code depending on read and push issue  LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */
int PMReadHitStack(FILE *streamin, PMHitStack *stkp, Bool skipMark);

/*
 *@doc 
 *Read PMHitStack from file named filename and Push hits in a PMHitStack
 *@param char *filename (file containing hits)
 *@param PMHitStack *stkp (PMHitStack to fill with hits)
 *@param skipMark (skip % marks if TRUE)
 *@return error code depending on read and push issue  LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */
int PMLoadHitStack(char *filename, PMHitStack *stkp, Bool skipMark);

/*
 *@doc
 *Write a PMHitStack in a file
 *@param FILE *streamou (file to write), PMHitStack *stk (PMHitStack to write write in a file)
 *@return error code depending on write LX_NO_ERROR, LX_IO_ERROR
 */
int PMWriteHitStack(FILE *streamou, PMHitStack *stkp);

#endif











