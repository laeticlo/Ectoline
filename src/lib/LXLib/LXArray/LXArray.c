/* -----------------------------------------------------------------------
 * $Id: LXDynBuf.c 1044 2010-04-18 23:36:42Z viari $
 * -----------------------------------------------------------------------
 * @file: LXArray.c
 * @desc: dynamic array
 * 
 * @history:
 * @+ <Gloup> : Mar 10 : first version
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * dynamic array<br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXArray.h"

/* -------------------------------------------- */
/* internal utilities                           */
/* -------------------------------------------- */

/* -------------------------------------------- */
/* API                                          */
/* -------------------------------------------- */

LXArray *LXFreeArray(LXArray *arr) {
    IFF_RETURN(arr, NULL);
    IFFREE(arr->data);
    FREE(arr);
    return NULL;
}

/* -------------------------------------------- */

LXArray *LXNewArray(size_t sizelt, size_t initialCapacity) {

    LXArray *arr = NEW(LXArray);
    IFF_RETURN(arr, NULL);
    ZERO(arr);
    
    if (initialCapacity) {
        arr->data = LXMalloc(sizelt * initialCapacity);
        IFF_RETURN(arr->data, LXFreeArray(arr));
    }
    
    arr->sizelt = sizelt;
    arr->nbelt = 0; // actual number of elements
    arr->capacity = initialCapacity;
    return arr;
}

/* -------------------------------------------- */

int LXSetArrayCapacity(LXArray *arr, size_t capacity) {
    IFF_RETURN(arr, LX_ARG_ERROR);
    
    if (capacity) {
        if (arr->data) {
            void *ptr = LXRealloc(arr->data, arr->sizelt * capacity);
            IFF_RETURN(ptr, LX_MEM_ERROR);
            arr->data = ptr;
        }
        else {
            arr->data = LXMalloc(arr->sizelt * capacity);
            IFF_RETURN(arr->data, LX_MEM_ERROR);
        }
    }
    else {
        IFFREE(arr->data);
        arr->data = NULL;
    }
    
    arr->nbelt = MIN(arr->nbelt, capacity);
    arr->capacity = capacity;
    
    return LX_NO_ERROR;
}

/* -------------------------------------------- */

int LXAssertArrayCapacity(LXArray *arr, size_t capacity) {
    IFF_RETURN(arr, LX_ARG_ERROR);
    if (arr->capacity < capacity)
        return LXSetArrayCapacity(arr, 2 * capacity);
    return LX_NO_ERROR;
}

/* -------------------------------------------- */

int LXAppendArray(LXArray *dst, const void *src, size_t nbelt) {
    IFF_RETURN(dst, LX_ARG_ERROR);
    IFF_RETURN(nbelt, LX_NO_ERROR);
    
    int stat = LXAssertArrayCapacity(dst, dst->nbelt + nbelt);
    IFF_RETURN(stat == LX_NO_ERROR, stat);
    
    (void) memcpy(((char *) dst->data) + (dst->sizelt * dst->nbelt),
                  src, (dst->sizelt * nbelt));
    dst->nbelt += nbelt;

    return LX_NO_ERROR;
}

/* -------------------------------------------- */

int LXSortArray(LXArray *arr, int (*compare)(const void*, const void*)) {
    IFF_RETURN(arr, LX_ARG_ERROR);
    IFF_RETURN(compare, LX_ARG_ERROR);

    qsort(arr->data, arr->nbelt, arr->sizelt, compare);

    return LX_NO_ERROR;
}

