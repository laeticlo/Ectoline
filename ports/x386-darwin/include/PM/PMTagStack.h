/* -----------------------------------------------------------------------
 * $Id: PMTagStack.h 1935 2013-07-25 12:38:55Z viari $
 * -----------------------------------------------------------------------
 * @file: PMTagStack.h
 * @desc: stack of tags library
 *
 * @history:
 * @+ <Wanou>  : Jan 03 : first version
 * @+ <Gloup>  : Dec 05 : major revision
 * @+ <Gloup>  : Feb 06 : code cleanup
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * stack of tags management library<br>
 * specialization of LXStackp
 *
 * @docend:
 */

#ifndef _H_PMTagStack
#define _H_PMTagStack

#include "LX.h"

#include "PMTag.h"

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/* ---------------------------------------------------- */
/* Types & Data Structures                              */
/* ---------------------------------------------------- */

/*
 * @doc: sort type 
 * - sortTagsById    : selector for sorting tags according their ids (spectId+id)
 * - sortTagsBySeq   : selector for sorting tags according their sequences
 * - sortTagsByMn    : selector for sorting tags according their N term masses
 * - sortTagsByMc    : selector for sorting tags according their C term masses
 * - sortTagsByScore : selector for sorting tags according their scores
 */
typedef enum {
  sortTagsById = 0,
  sortTagsBySeq,
  sortTagsByMn,
  sortTagsByMc,
  sortTagsByScore
} PMTagSortType;

/* 
 * @doc: TagStack types (just aliases)
 */

typedef LXStackp     PMTagStack; 
typedef LXStackpPtr  PMTagStackPtr; 
typedef LXStackpHdle PMTagStackHdle; 

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 *@doc 
 *Create a new (empty) PMTagStack
 *@return new PMTagStack*
 */

PMTagStack *PMNewTagStack(void);

/*
 *@doc 
 *Free a PMTagStack (freeing tags too)
 *@param PMTagStack *stk (PMTagStack to free) 
 *@return NULL;
 */

PMTagStack *PMFreeTagStack(PMTagStack *stkp);

/*
 *@doc 
 *Sort a PMTagStack according to tag attributes
 *@param PMTagStack *stk (PMTagStack to sort) 
 *@param TagSortType sorting criterion
 */

void PMSortTagStack(PMTagStack *stk, PMTagSortType sortBy);

/*
 *@doc 
 *Read PMTagStack from file and Push tags in a PMTagStack
 *@param FILE *streamin (file containing Tags)
 *@param PMTagStack *stkp (PMTagStack to fill with tags)
 *@param skipMark (skip % marks if TRUE)
 *@return error code depending on read and push issue  LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */
int PMReadTagStack(FILE *streamin, PMTagStack *stkp, Bool skipMark);

/*
 *@doc 
 *Read PMTagStack from file named filename and Push tags in a PMTagStack
 *@param char *filename (file containing Tags)
 *@param PMTagStack *stkp (PMTagStack to fill with tags)
 *@param skipMark (skip % marks if TRUE)
 *@return error code depending on read and push issue  LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */
int PMLoadTagStack(char *filename, PMTagStack *stkp, Bool skipMark);

/*
 *@doc
 *Write a PMTagStack in a file
 *@param FILE *streamou (file to write), PMTagStack *stk (PMTagStack to write write in a file)
 *@return error code depending on write LX_NO_ERROR, LX_IO_ERROR
 */
int PMWriteTagStack(FILE *streamou, PMTagStack *stkp);

#endif











