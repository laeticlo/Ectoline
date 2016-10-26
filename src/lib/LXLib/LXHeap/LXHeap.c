/* -----------------------------------------------------------------------
 * $Id: LXHeap.c 1782 2013-02-23 00:11:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXHeap.c
 * @desc: LXHeap utility. 
 * 
 * @history:
 * @+ <Gloup> : Jan 96 : first version for PWG 
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart:
 *
 *  LXHeap utility library<br>
 *  This heap is used to keep track of n best elements
 *  when scanning N (>> n) elements. The heap update is in log(n)<br>
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXSys.h"
#include "LX/LXHeap.h"

#define FATHER(child)  ((child) - 1) / 2
#define CHILD(father)  (2 * (father)) + 1
#define BROTHER(child) (child) + 1

#define FREE_NODE(p, freeFunc) if (freeFunc) freeFunc(p)

/* ------------------------------------------------------------ */
/* static utilities                                             */
/* ------------------------------------------------------------ */

/* ---------------------------- */
/* Heap update upwards          */
/* ---------------------------- */

static void sUpdateUp(LXHeap *heap) {
    size_t  child, father;
    void    *tmp;

    child = heap->last - 1;
    father = FATHER(child);
    tmp = heap->node[child];
    
    while (   (child > 0)
           && (heap->compare(tmp, heap->node[father]) < 0)) {
        heap->node[child] = heap->node[father];
        child = father;
        father = FATHER(child);
    }
    
    heap->node[child] = tmp;
}

/* ---------------------------- */
/* Heap update downwards        */
/* ---------------------------- */

static void sUpdateDown(LXHeap *heap) {
    size_t father, child;
    void   *tmp;

    father = 0;
    child = 1;
    
    tmp = heap->node[father];
    
    while (child < heap->last) {
        size_t next = child;
        size_t brother = BROTHER(child);
        if (brother < heap->last) {     /* 2 children : choose the smallest */
            if (heap->compare(heap->node[child], heap->node[brother]) > 0)
                next = brother;
        }
        if (heap->compare(tmp, heap->node[next]) > 0) {
            heap->node[father] = heap->node[next];
            father = next;
            child = CHILD(father);
        }
        else
            break;
    }
    
    heap->node[father] = tmp;
}


/* ------------------------------------------------------------ */
/* HEAP API                                                     */
/* ------------------------------------------------------------ */

/* -------------------------------------------- */
/* destroy Heap                                 */
/* do not destroy pointers if freeFunc == null  */
/* -------------------------------------------- */

LXHeap *LXFreeHeap(LXHeap *heap) {
    IFF_RETURN(heap, NULL);
    
    if (heap->node) {

      if (heap->free) {
        size_t i;
        for (i = 0 ; i < heap->last ; i++) {
          if (heap->node[i])
            heap->free(heap->node[i]);
        }
      }

      FREE(heap->node);
    }

    FREE(heap);
    
    return NULL;
}

/* -------------------------------------------- */
/* Create Heap                                  */
/* -------------------------------------------- */

LXHeap *LXNewHeap(size_t size, int (*compare)(const void*, const void*),
                               void (*ffree)(void*)) {
    IFF_RETURN(size > 0, NULL);
    IFF_RETURN(compare, NULL);

    size_t  i;
    LXHeap *heap;

    if (! (heap = NEW(LXHeap)))
        return NULL;

    heap->size    = size;
    heap->last    = 0;
    heap->compare = compare;
    heap->free    = ffree;
    
    heap->node = NEWN(void*, size);

    if (! heap->node) 
       return LXFreeHeap(heap);
       
    for (i = 0 ; i < size ; i++)
      heap->node[i] = NULL;
        
    return heap;
}

/* ---------------------------- */
/* Add item in heap             */
/* ---------------------------- */

void LXAddInHeap(LXHeap *heap, void *item) {
    if (! heap) return;
    
    if (heap->last < heap->size) {             /* heap not full yet     */
    
        heap->node[heap->last] = item;
        heap->last++;
        sUpdateUp(heap);
    }
    else if (heap->compare(item, heap->node[0]) > 0) { /* item is better       */
    
        FREE_NODE(heap->node[0], heap->free);
        heap->node[0] = item;
        sUpdateDown(heap);
    }
    else {                                      /* item is worst        */
        FREE_NODE(item, heap->free);
    }
}

/* ---------------------------- */
/* get root value               */
/* ---------------------------- */

void *LXGetHeapRoot(LXHeap *heap) {
    return (heap ? heap->node[0] : NULL);
}

/* ---------------------------- */
/* Pop root value, and update   */
/* ---------------------------- */

void *LXPopHeapRoot(LXHeap *heap) {

    IFF_RETURN(heap, NULL);
    
    void *tmp;
  
    if (heap->last == 0)
        return NULL;
    
    tmp = heap->node[0];

    heap->node[0] = heap->node[--(heap->last)];

    sUpdateDown(heap);
  
    return tmp;
}

/* -----------------------------------------------  */
/* Rebuilt the heap according to another            */
/* comparFunc (in nlogn)                            */
/* -----------------------------------------------  */

int LXRebuiltHeap(LXHeap *heap, int (*compare)(const void*, const void*)) {

    IFF_RETURN(heap, LX_ARG_ERROR);
    IFF_RETURN(compare, LX_ARG_ERROR);

    heap->compare = compare;
    
    if (heap->last == 0)
      return LX_NO_ERROR; // no error but nothing to do

    LXHeap *tmp;
    if (! (tmp = LXNewHeap(heap->last, compare, NULL)))
      return LX_MEM_ERROR;

    size_t i;
    for (i = 0 ; i < heap->last ; i++) 
      LXAddInHeap(tmp, heap->node[i]);
    
    void **vmp = heap->node;
    heap->node = tmp->node;
    tmp->node = vmp;

    LXFreeHeap(tmp);
    

    return LX_NO_ERROR;
}
