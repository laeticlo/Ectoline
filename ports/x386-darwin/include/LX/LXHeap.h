/* -----------------------------------------------------------------------
 * $Id: LXHeap.h 1110 2010-05-16 16:33:11Z viari $
 * -----------------------------------------------------------------------
 * @file: LXHeap.h
 * @desc: Helix heap utility
 * 
 * @history:
 * @+ <Gloup> : Jan 96 : first version for PWG 
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * Helix heap utility library<br>
 *
 *
 * @docend:
 */

#ifndef _H_LXHeap

#define _H_LXHeap

#ifndef _H_LXTypes
#include "LXTypes.h"
#endif

#ifndef _H_LXMacros
#include "LXMacros.h"
#endif

/* ---------------------------------------------------- */
/* Structures                                           */
/* ---------------------------------------------------- */

/*
 * @doc: LXHeap structure
 * - size    : size_t heap capacity
 * - last    : size_t index of current last item in heap (private)
 * - compare : function to compare items : int compare(const void*, const void*)
 *             (same as in qsort(2))
 * - free    : function used to free items (use NULL not to free them)
 * - node    : void** internal array of pointers to nodes (private)
 */

typedef struct {
    size_t    size;               /* heap capacity                         */
    size_t    last;               /* number of items                       */
    int       (*compare)(const void*, const void*); /* comparison function */
    void      (*free)(void*);     /* free function                         */
    void      **node;             /* internal - array of item              */
} LXHeap;


/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * Free previously allocated LXHeap
 * @param heap LXHeap* heap to free
 * @return NULL
 */
LXHeap *LXFreeHeap(LXHeap *heap);

/*
 * @doc: 
 * Create a new empty LXHeap
 * @param size requested capacity of heap (should be > 0)
 * @param compare function used to compare items (same as in qsort(2))
 * @param free function used to free items (use NULL not to free them)
 * @return new LXHeap or NULL on error
 */
LXHeap *LXNewHeap(size_t size, int (*compare)(const void*, const void*),
                  void (*free)(void*));

/*
 * @doc: 
 * Add an item in heap
 * @param heap LXHeap* heap to modify
 * @param item void* item to add
 */
void LXAddInHeap(LXHeap *heap, void *item);

/*
 * @doc: 
 * Pop heap root item and update heap. heap root item is the smallest heap
 * item according to heap compare function.
 * @param heap LXHeap* heap to modify
 * @return root value (you are responsible to further free it, if needed)
 */
void *LXPopHeapRoot(LXHeap *heap);

/*
 * @doc: 
 * get heap current root item i.e. smallest item according to heap
 * compare function. The heap is unaltered... so don't modify or free
 * this value or the heap will get corrupted. If you want to modify the value,
 * first pop it from heap using LXPopHeapRoot and put it back again using
 * LXAddInHeap.
 * @param heap LXHeap* heap to sneak
 * @return root value (don't modify it !)
 */
void *LXGetHeapRoot(LXHeap *heap);


/*
 * @doc: 
 * Rebuilt the heap according to another comparFunc (in n.logn). On 
 * succesfull call (LX_NO_ERROR), the heap compare function will be changed
 * to argument.
 * @param heap LXHeap* heap to rebuilt
 * @param compare new function used to compare items (same as in qsort(2))
 * @return LX_NO_ERROR if ok, LX_xxx error code on error
 */
int LXRebuiltHeap(LXHeap *heap, int (*compare)(const void*, const void*));


#endif



