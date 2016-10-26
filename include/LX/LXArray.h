/* -----------------------------------------------------------------------
 * $Id: LXArray.h 1127 2010-06-16 17:00:52Z viari $
 * -----------------------------------------------------------------------
 * @file: LXArray.h
 * @desc: dynamic array utilities
 * 
 * @history:
 * @+ <Gloup> : Jan 10 : first version
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * dynamic array utilities<br>
 * dynamically sizeable array of elements. 
 * an array contain any kind of data (element).<br>
 * an array is created by 
 * <code>LXNewArray(size_t sizelt, size_t initialCapacity)</code>
 * where <code>sizelt</code> is the size of an element 
 * and <code>initialCapacity</code> is the initial array capacity
 * (may be 0).<br>
 * Element are added by <bold>copying</bold> a source array of
 * elements into the destination array by <bold>LXAppendArray</code>.
 * Note: no indexed access function or iterators are provided since
 * the easiest way in C is to cast <code>array->data</code> to proper
 * element type.<br>
 * example: <br>
 * <code>
 * 
 * typedef struct {
 *     int x, y;
 * } Point;
 * 
 * int compare(const void *p1, const void *p2) {
 *     return ((Point *) p2)->x - ((Point *) p1)->x;
 * }
 * 
 * main() {
 * 
 *   LXArray *buf = LXNewArray(sizeof(Point), 0);
 *   
 *   int i;
 *   
 *   for (i = 0; i < 10 ; i++) {
 *     Point p = {i, 2*i};
 *     LXAppendArray(buf, &p, 1);
 *   }
 *   
 * 
 *   LXSortArray(buf, compare); // sort by decreasing order of x
 *   
 *   Point *pt = (Point *) buf->data;
 *   
 *   for (i = 0 ; i < buf->nbelt ; i++) {
 *     printf("%d : %d %d\n", i, pt[i].x, pt[i].y);
 *   }
 * 
 *   (void) LXFreeArray(buf);
 *   
 *   exit(0);  
 * }
 * </code>
 *
 * @docend:
 */

#ifndef _H_LXArray
#define _H_LXArray

#ifndef _H_LXSys
#include "LXSys.h"
#endif

#include <stdio.h>

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/* ---------------------------------------------------- */
/* Macros                                               */
/* ---------------------------------------------------- */

/* ---------------------------------------------------- */
/* Structures                                           */
/* ---------------------------------------------------- */

/*
 * @doc: Dynamic Array
 * - nbelt  : size_t current number of elements
 * - sizelt : size_t size of an element
 * - data   : current data buffer
 * - capacity : size_t - internal - current buffer capacity (in elements)
 */
 
typedef struct {
    size_t  nbelt;              // current number of elements
    size_t  sizelt;             // size of an element
    void   *data;               // data buffer
    size_t  capacity;           // -internal - current buffer capacity (in elements)
} LXArray, *LXArrayPtr;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * free a previously allocated array.
 * note: you are responsible of freeing elements if needed.
 * @param arr LXArray* array to free
 * @return NULL
 */

LXArray *LXFreeArray(LXArray *arr);

/*
 * @doc: 
 * allocate a new array.
 * @param sizelt size_t element size (in bytes)
 * @param nbelt size_t initial capacity (nb elts) (may be 0)
 * @return new array (NULL on memory error)
 */

LXArray *LXNewArray(size_t sizelt, size_t initialCapacity);

/*
 * @doc: 
 * resize array to given capacity.
 * note: ususally this function is only called internally.
 * @param arr LXArray* array to resize
 * @param capacity size_t new capacity (number of elts, may be 0)
 * @return LX_NO_ERROR if ok, LX_ARG_ERROR if arr is NULL, LX_MEM_ERROR on memory shortage
 */

int LXSetArrayCapacity(LXArray *arr, size_t capacity);

/*
 * @doc: 
 * check that array capacity is enough for 'capacity' total number of elements
 * and increase capacity as needed.
 * note: ususally this function is only called internally.
 * @param arr LXArray* array to resize
 * @param capacity size_t total number of elts
 * @return LX_NO_ERROR if ok, LX_ARG_ERROR if arr is NULL, LX_MEM_ERROR on memory shortage
 */

int LXAssertArrayCapacity(LXArray *arr, size_t capacity);

/*
 * @doc: 
 * append elements from src into dst array (increase dst size if needed).
 * @param dst LXArray* destination array.
 * @param src void* source data to append. This is a pointer to an array
 * of <code>nbelt</code> which will be <bold>copied</bold> into dst.
 * note: To add a single element, pass adress of element as <code>src</code>
 * and <code>1</code> as nbelt.
 * @param nbelt size_t number of elts in src (each element if supposed to be dst->sizelt bytes).
 * @return LX_NO_ERROR if ok, LX_ARG_ERROR if arr is NULL, LX_MEM_ERROR on memory shortage
 */

int LXAppendArray(LXArray *dst, const void *src, size_t nbelt);

/*
 * @doc: 
 * sort elements in array according to 'compare' function.
 * @param arr LXArray* array to resize
 * @param compare function as in qsort(2). The arguments of <code>compare</code>
 * are pointers to elements.
 * @return LX_NO_ERROR if ok, LX_ARG_ERROR if arr is NULL, LX_MEM_ERROR on memory shortage
 */

int LXSortArray(LXArray *arr, int (*compare)(const void*, const void*));

#endif
