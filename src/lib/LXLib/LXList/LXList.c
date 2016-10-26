/* -----------------------------------------------------------------------
 * $Id: LXList.c 1782 2013-02-23 00:11:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXList.c
 * @desc: double linked list library. 
 * 
 * @history:
 * @+ <Gloup> : Jan 96 : first version for PWG 
 * -----------------------------------------------------------------------
 * @docstart:
 *
 *  Double linked list library<br>
 *
 * TODO: documentation
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXSys.h"
#include "LX/LXList.h"

#define FUNC_RETURN(ptr, value) if (! (ptr)) return (value)
#define NULL_RETURN(ptr) if (! (ptr)) return NULL
#define VOID_RETURN(ptr) if (! (ptr)) return

/* ------------------------------------------------------------ */
/* static utilities                                             */
/* ------------------------------------------------------------ */

static LXCellPtr sSelectedCell;
static LXCellCompareFunc sCellComparFunc;

static LXCellPtr sEndOfList(LXCellPtr root) {
  while (root->next)
    root = root->next;
  return root;
}

static LXCellPtr sStartOfList(LXCellPtr root) {
  while (root->previous)
    root = root->previous;
  return root;
}

static void *sNoCopy(void *body) {
  return body;
}

static int sCompareCellPtr(const void *p1, const void *p2) {
    return sCellComparFunc(*((const LXCellPtr *) p1), *((const LXCellPtr *) p2));
}

static int sSelectByCell(LXCellPtr ref) {
  return (ref == sSelectedCell);
}

/* ------------------------------------------------------------ */
/* LIST API                                                     */
/* ------------------------------------------------------------ */

/* -------------------------------------------- */
/* -------------------------------------------- */

LXCellPtr LXNewCell(void *body) {
  LXCellPtr new = NEW(LXCell);
  NULL_RETURN(new);
  new->body = body;
  new->previous = new->next = NULL;
  return new;
}

/* -------------------------------------------- */

void LXFreeCell(LXCellPtr cell, LXBodyDestroyFunc destroyFun) {
  VOID_RETURN(cell);
  if (destroyFun)
    destroyFun(cell->body);
  FREE(cell);
}

/* -------------------------------------------- */

void LXFreeCells(LXCellPtr *root, LXBodyDestroyFunc destroyFun) {
  LXCellPtr curr, save;
  for (curr = *root; curr; curr = save) {
    save = curr->next;
    LXFreeCell(curr, destroyFun);
  }
  *root = NULL;
}

/* -------------------------------------------- */
/* -------------------------------------------- */

LXCellPtr LXAddCell(LXCellPtr *root, LXCellPtr cell) {
  NULL_RETURN(cell);
  if (*root) {
    LXCellPtr last = sEndOfList(*root);
    last->next = cell;
    cell->previous = last;
    cell->next = NULL;
  }
  else {
    *root = cell;
    cell->previous = cell->next = NULL;
  }
  return cell;
}

LXCellPtr LXRemoveCell(LXCellPtr *root, LXCellPtr cell) {
  NULL_RETURN(cell);
  if (cell == *root)
    *root = cell->next;
  return LXDisconnectCell(cell);
}

/* -------------------------------------------- */
/* -------------------------------------------- */

LXCellPtr LXInsertCellAfter(LXCellPtr ref, LXCellPtr cell) {
  NULL_RETURN(ref);
  NULL_RETURN(cell);
  cell->next = ref->next;
  cell->previous = ref;
  if (ref->next)
    ref->next->previous = cell;
  ref->next = cell;
  return cell;
}

/* -------------------------------------------- */

LXCellPtr LXInsertCellBefore(LXCellPtr ref, LXCellPtr cell) {
  NULL_RETURN(ref);
  NULL_RETURN(cell);
  cell->previous = ref->previous;
  cell->next = ref;
  if (ref->previous)
    ref->previous->next = cell;
  ref->previous = cell;
  return cell;
}


LXCellPtr LXDisconnectCell(LXCellPtr cell) {
  NULL_RETURN(cell);
  if (cell->previous)
    cell->previous->next = cell->next;
  if (cell->next)
    cell->next->previous = cell->previous;
  cell->next = cell->previous = NULL;
  return cell;
}

/* -------------------------------------------- */
/* -------------------------------------------- */

size_t LXNumberOfCells(LXCellPtr ref) {
  int count = 0;
  while (ref) {
    count++;
    ref = ref->next;
  }
  return count;
}

/* -------------------------------------------- */
/* -------------------------------------------- */

LXCellPtr LXLastCell(LXCellPtr ref) {
  NULL_RETURN(ref);
  return sEndOfList(ref);
}

/* -------------------------------------------- */

LXCellPtr LXFirstCell(LXCellPtr ref) {
  NULL_RETURN(ref);
  return sStartOfList(ref);
}

/* -------------------------------------------- */

int LXIsCellBefore(LXCellPtr ref, LXCellPtr cell) {
  sSelectedCell = cell;
  return (LXFindCell(ref, LX_SEARCH_UPSTREAM, sSelectByCell) != NULL);
}

/* -------------------------------------------- */

int LXIsCellAfter(LXCellPtr ref, LXCellPtr cell) {
  sSelectedCell = cell;
  return (LXFindCell(ref, LX_SEARCH_DOWNSTREAM, sSelectByCell) != NULL);
}

/* -------------------------------------------- */
/* -------------------------------------------- */

LXCellPtr LXFindCell(LXCellPtr ref, int searchPath, LXCellSelectFunc selectFun)
{
  switch (searchPath) {
  
    case LX_SEARCH_UPSTREAM :
      while (ref) {
          if (selectFun(ref))
            return ref;
          ref = ref->previous;
      }
      break;
      
    default:
      while (ref) {
          if (selectFun(ref))
            return ref;
          ref = ref->next;
      }
      break;
   }
  return NULL;
}

/* -------------------------------------------- */

LXCellPtr LXFindCells(LXCellPtr ref, int searchPath, LXCellSelectFunc selectFun)
{
  LXCellPtr last, root = NULL;
  
  switch (searchPath) {
  
    case LX_SEARCH_UPSTREAM :
      while (ref) {
          if (selectFun(ref))
            last = LXAddCell(root ? &last : &root, LXNewCell(ref->body));
          ref = ref->previous;
      }
      break;
      
    default:
      while (ref) {
          if (selectFun(ref))
            last = LXAddCell(root ? &last : &root, LXNewCell(ref->body));
          ref = ref->next;
      }
      break;
   }
   
  return root;
}

/* -------------------------------------------- */
/* -------------------------------------------- */

LXCellPtr LXSortCells(LXCellPtr *root, LXCellCompareFunc comparFun) {

  // -------------------------------
  // check args
  //
  NULL_RETURN(root);
  size_t size = LXNumberOfCells(*root);
  if (size == 0)
    return *root;

  // -------------------------------
  // record comparison function
  //
  sCellComparFunc = comparFun;
  
  // -------------------------------
  // allocate temporary array
  //
  LXCellPtr *cellArray = NEWN(LXCellPtr, size+1);
  NULL_RETURN(cellArray);

  // -------------------------------
  // copy cells into array
  //
  size_t icell;
  LXCellPtr src = *root;
  for (icell = 0 ; icell < size ; icell++) {
    cellArray[icell] = src;
    src = src->next;
  }

  // -------------------------------
  // sort array
  //
  qsort(cellArray, size, sizeof(LXCellPtr), sCompareCellPtr);

  // -------------------------------
  // replace original root
  // and link ordered cells properly
  //
  *root = cellArray[0];
  for (icell = 1 ; icell < size ; icell++) {
    cellArray[icell-1]->next = cellArray[icell];
    cellArray[icell]->previous = cellArray[icell - 1];
  }
  cellArray[size-1]->next = NULL;

  // -------------------------------
  // free temporary array
  //
  FREE(cellArray);

  return *root;
}

/* -------------------------------------------- */
/* -------------------------------------------- */

LXCellPtr LXCopyList(LXCellPtr src, LXBodyCopyFunc copyFun)
{
  LXCellPtr last, root = NULL;
  
  if (! copyFun)
    copyFun = sNoCopy;
    
  while (src) {
    last = LXAddCell(root ? &last : &root, LXNewCell(copyFun(src->body)));
    src = src->next;
  }
  return root;
}

/* -------------------------------------------- */

size_t LXNumberOfSelectedCells(LXCellPtr ref, LXCellSelectFunc selectFun) {
  size_t count = 0;
  while (ref) {
    if (selectFun(ref))
        count++;
    ref = ref->next;
  }
  return count;
}

/* -------------------------------------------- */

size_t LXFreeSelectedCells(LXCellPtr *root, LXCellSelectFunc selectFun, LXBodyDestroyFunc destroyFun) {
  FUNC_RETURN(root, 0);

  size_t count = 0;
  LXCellPtr curr = *root;
  
  while (curr) {
    if (selectFun(curr)) {
    
      if (curr == *root)
        *root = curr->next;
      else
        curr->previous->next = curr->next;

      if (curr->next)
        curr->next->previous = curr->previous;
        
      LXCellPtr save = curr->next;
      
      LXFreeCell(curr, destroyFun);
      
      count++;
      curr = save;
    }
    else {
      curr = curr->next;
    }
  }
  return count;
}

