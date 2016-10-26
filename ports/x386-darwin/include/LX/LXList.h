/* -----------------------------------------------------------------------
 * $Id: LXList.h 1793 2013-02-24 02:19:06Z viari $
 * -----------------------------------------------------------------------
 * @file: LXList.h
 * @desc: double linked list library.
 * 
 * @history:
 * @+ <Gloup> : Jan 96 : first version for PWG 
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * Helix double linked list library<br>
 *
 * TODO: documentation
 *
 * @docend:
 */

#ifndef _H_LXList

#define _H_LXList

#ifndef _H_LXTypes
#include "LXTypes.h"
#endif

#ifndef _H_LXMacros
#include "LXMacros.h"
#endif

/*
 * @doc: constant to indicate search direction in LXList
 * - LX_SEARCH_UPSTREAM   : search upstream
 * - LX_SEARCH_DOWNSTREAM : search downstream
 */

#define LX_SEARCH_UPSTREAM		    0
#define LX_SEARCH_DOWNSTREAM		1

/* ---------------------------------------------------- */
/* Structures                                           */
/* ---------------------------------------------------- */

/*
 * @doc: List Cell structure
 * - body       : cell content (any pointer)
 * - next       : pointer to next cell in list (private)
 * - previous   : pointer to previous cell in list (private)
 */

typedef struct s_Cell {
    void *body;
    struct s_Cell *next;
    struct s_Cell *previous;
} LXCell, *LXCellPtr;

/*
 * @doc: function to handle cell body
 * - body       : cell content (any pointer)
 * - next       : pointer to next cell in list (private)
 * - previous   : pointer to previous cell in list (private)
 */


typedef int     (*LXCellSelectFunc)  (const LXCellPtr cell);
typedef int     (*LXCellCompareFunc) (const LXCellPtr cell_1, const LXCellPtr cell_2);
typedef void    (*LXBodyDestroyFunc) (void *body);
typedef void*   (*LXBodyCopyFunc)    (void *body);

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: TODO
 * @param type xxx
 * @return NULL
 */

LXCellPtr   LXNewCell       (void *body);
void        LXFreeCell      (LXCellPtr cell, LXBodyDestroyFunc destroyFun);
void        LXFreeCells     (LXCellPtr *root, LXBodyDestroyFunc destroyFun);

LXCellPtr   LXAddCell           (LXCellPtr *root, LXCellPtr cell);
LXCellPtr   LXRemoveCell        (LXCellPtr *root, LXCellPtr cell);

LXCellPtr   LXInsertCellAfter   (LXCellPtr ref, LXCellPtr cell);
LXCellPtr   LXInsertCellBefore  (LXCellPtr ref, LXCellPtr cell);
LXCellPtr   LXDisconnectCell    (LXCellPtr cell);

size_t      LXNumberOfCells     (LXCellPtr ref);

LXCellPtr   LXLastCell          (LXCellPtr ref);
LXCellPtr   LXFirstCell         (LXCellPtr ref);
int         LXIsCellBefore      (LXCellPtr ref, LXCellPtr cell);
int         LXIsCellAfter       (LXCellPtr ref, LXCellPtr cell);

LXCellPtr   LXFindCell          (LXCellPtr ref, int searchPath, LXCellSelectFunc selectFun);
LXCellPtr   LXFindCells         (LXCellPtr ref, int searchPath, LXCellSelectFunc selectFun);

LXCellPtr   LXSortCells         (LXCellPtr *root, LXCellCompareFunc comparFun);

LXCellPtr   LXCopyList          (LXCellPtr src, LXBodyCopyFunc copyFun);

size_t      LXNumberOfSelectedCells (LXCellPtr ref, LXCellSelectFunc selectFun);
size_t      LXFreeSelectedCells     (LXCellPtr *root, LXCellSelectFunc selectFun, LXBodyDestroyFunc destroyFun);

#endif

