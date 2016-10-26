/* -----------------------------------------------------------------------
 * $Id: LXStkp.h 1104 2010-05-15 21:28:33Z viari $
 * -----------------------------------------------------------------------
 * @file: LXStkp.h
 * @desc: stacks of pointers library 
 * 
 * @history:
 * @+ <Gloup> : Mar 92 : first draft
 * @+ <Gloup> : Aug 93 : added xxxVector funcs
 * @+ <Wanou> : Dec 01 : adapted to LX_Ware 
 * @+ <Gloup> : Aug 06 : added direct access funcs
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * @+ <Gloup> : Mar 10 : changed to size_t and removed unnecessary handles
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LXStkp library<br>
 * library of dynamic pointer stacks
 *
 * @docend:
 */

#ifndef _H_LXStkp

#define _H_LXStkp

#ifndef _H_LXSys
#include "LXSys.h"
#endif

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @doc: default minimum stack size
 */

#define LX_STKP_MIN_SIZE        2

/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/*
 * @doc: Pointer Stack structure
 * - size    : size_t - stack size
 * - top     : size_t - current free pos 
 * - cursor  : size_t - current cursor
 * - val     : Ptr* - values 
 */

typedef struct {
        size_t   size;           /* stack size           */
        size_t   top;            /* current free pos.    */
        size_t   cursor;         /* current cursor       */
        Ptr      *val;           /* values               */
} LXStackp, *LXStackpPtr, **LXStackpHdle;        


/*
 * @doc: typedef to destroy and compare functions
 *       (to help your casting)
 */
 
typedef void (*LXStackDestroy) (Ptr);
typedef int  (*LXStackCompar)  (const Ptr, const Ptr);

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * get (and optionally reset) current error code
 * @param reset Bool reset error code (to LX_NO_ERROR)
 * @return current error code (LX_XXX_ERROR)
 */
                                        
int LXStkpError (Bool reset);

/*
 * @doc: 
 * Allocate a new empty LXStackp structure
 * @param size size_t stack size
 * @return LXStackpPtr
 */

LXStackpPtr LXNewStackp (size_t size);

/*
 * @doc: 
 * Free a previously allocated LXStackp structure
 * @param stkp LXStackpPtr stack to free
 * @param destroy function to free each stack values
 * void destroy(Ptr p) or NULL
 * if destroy == NULL then values are not destroyed
 * @return NULL
 */

LXStackpPtr LXFreeStackp (LXStackpPtr stkp, void (*destroy)(Ptr));

/*
 * @doc: 
 * Resize a previously allocated LXStackp
 * @param stkp LXStackpPtr stack to resize
 * @param size size_t new size
 * @return size_t (size or error)
 */

size_t LXResizeStackp (LXStackpPtr stkp, size_t size);

/*
 * @doc: 
 * Copy a LXStackp
 * @param stkp LXStackpPtr stack to copy
 * @param copy function function to copy values
 * Ptr copy(Ptr p) or NULL
 * if copy == NULL then don't copy values just pointers
 * else copy should return a copy of the argument
 * @return LXStackpPtr
 */

LXStackpPtr LXCopyStackp (LXStackpPtr stkp, Ptr (*copy)(Ptr));

/*
 * @doc: 
 * Allocate a LXStackp Vector
 * @param vectSize size_t vector size
 * @param stackSize size_t size of each stack
 * @return LXStackpHdle
 */

LXStackpHdle LXNewStackpVector (size_t vectSize, size_t stackSize);

/*
 * @doc: 
 * Free a previously allocated LXStackp Vector
 * @param stkh LXStackpHdle vector to free
 * @param vectsize size_t vector size
 * @param destroy function to free each stack values
 * void destroy(Ptr p) or NULL
 * if destroy == NULL then values are not destroyed
 * @return NULL
 */

LXStackpHdle LXFreeStackpVector(LXStackpHdle stkh, size_t vectSize, void (*destroy)(Ptr));

/*
 * @doc: 
 * Push a value in the selected stack
 * @param stkp LXStackpPtr stack to actualize
 * @param val Ptr value to push
 * @return False if the selected LXStackp can not be expanded
 */

Bool LXPushpIn (LXStackpPtr stkp, Ptr val);

/*
 * @doc: 
 * Pop an pointer out of the selected stack
 * @param stkp LXStackpPtr stack to actualize
 * @param val Ptr* value to pop out (may be NULL)
 * @return False if the selected LXStackp is empty
 */

Bool LXPoppOut (LXStackpPtr stkp, Ptr *val);

/*
 * @doc: 
 * Read a Stack from top to bottom
 * @param stkp LXStackpPtr to read
 * @param val Ptr* value read
 * @return False if the LXStackp cursor is out of the stack range
 */

Bool LXReadpDown(LXStackpPtr stkp, Ptr *val);

/*
 * @doc: 
 * Read a LXStackp from bottom to the top
 * @param stkp LXStackpPtr to read
 * @param val Ptr* value to read
 * @return False if the LXStackp cursor is out of the stack range
 */

Bool LXReadpUp (LXStackpPtr stkp, Ptr *val);

/*
 * @doc: 
 * Put the LXStackp cursor to the top of the stack
 * @param stkp LXStackPtr to actualize
 */

void LXCurspToTop (LXStackpPtr stkp);

/*
 * @doc: 
 * Put the LXStackp cursor to the bottom of the stack
 * @param stkp LXStackPtr to actualize
 */

void LXCurspToBottom (LXStackpPtr stkp);

/*
 * @doc: 
 * Swap cursor value and top-1 value in the stack
 * @param stkp LXStackPtr to modify
 */

void LXCurspSwap (LXStackpPtr stkp);

/*
 * @doc: 
 * get current cursor position
 * @param stkp LXStackPtr
 * @return current cursor position;
 */

int LXCurspGetPos (LXStackpPtr stkp);

/*
 * @doc: 
 * get value at current cursor position
 * @param stkp LXStackPtr
 * @return value at current cursor position;
 */

Ptr LXCurspGetValue (LXStackpPtr stkp);

/*
 * @doc: 
 * Search for a value in stack starting from current
 * cursor position and going downward. When a value
 * is found, then return True and leave cursor at the
 * matching position.
 * note: this is an O(n) search. if the stack
 * has been previously sorted, you may use LXBinSearchStackp instead.
 *
 * @param stkp LXStackpPtr to search in 
 * @param sval Ptr value to be found
 * @param compar function comparison function. 
 * int compar(Ptr p1, Ptr p2) or NULL
 * if compar == NULL then use equality of adresses.
 * if compar != NULL then it should return 0 if the two arguments are equal.
 * note that arguments of compar are Ptr
 * @return False if the given value can not be found in the stack.
 * 
 */

Bool LXSearchDownStackp (LXStackpPtr stkp, Ptr sval, int (*compar)(const void *, const void*));

/*
 * @doc: 
 * Perform dichotomic search in stack.
 * Values should have been sorted in descending order from top to bottom.
 * note: this is an O(logn) search.
 *
 * @param stkp LXStackpPtr to search in 
 * @param sval Ptr value to be found
 * @param compar function comparison function. 
 * int compar(Ptr p1, Ptr p2) should return <0, 0, >0 if
 * the first argument is less, equal, greater (resp.) than second.
 * note that arguments of compar are Ptr
 * @return False if the given value can not be found in the stack
 *         True if the value has been found and leave cursor at
 *         found position.
 * 
 */

Bool LXBinSearchStackp (LXStackpPtr stkp, Ptr sval, int (*compar)(const void *, const void*));

/*
 * @doc: 
 * sort values in stack in descending order from top to bottom 
 * according to comparison function.
 * note: (internally uses a quick sort that is *not* stable) 
 * @param stkp LXStackpPtr to sort
 * @param compar function comparison function. 
 * int compar(Ptr *p1, Ptr *p2) should return <0, 0, >0 if
 * the object pointed to by first argument is less, equal, greater (resp.) 
 * than the object pointed to by second argument.
 * note that arguments of compar are Ptr* (not Ptr)
 * 
 */

void LXSortStackp (LXStackpPtr stkp, int (*compar)(const void *, const void *));

/*
 * @doc: 
 * Invert the stack order
 * @return false if the stack is empty
 */

void LXReverseStackp (LXStackpPtr stkp);

/*
 * @doc: 
 * Get stack current size
 * @param stkp LXStackpPtr
 * @return stack size
 */

size_t LXStackpSize (LXStackpPtr stkp);

/*
 * @doc: 
 * Tell if stack is empty
 * @param stkp LXStackpPtr
 * @return TRUE if stack empty
 */

Bool LXIsStackpEmpty (LXStackpPtr stkp);

/*
 * @doc: 
 * Access to stack element at index. (0 index is bottom) 
 * @param stkp LXStackpPtr
 * @param indx size_t index to retrieve
 * @return value at index (NULL if index is invalid)
 */

Ptr LXStackpGetValue (LXStackpPtr stkp, size_t indx);

/*
 * @doc: 
 * Access to current top element.  
 * @param stkp LXStackpPtr
 * @return value at top (NULL if stack is empty)
 */

Ptr LXStackpGetTop (LXStackpPtr stkp);

#endif
