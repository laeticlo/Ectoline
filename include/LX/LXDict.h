/* -----------------------------------------------------------------------
 * $Id: LXDict.h 1112 2010-05-23 18:01:09Z viari $
 * -----------------------------------------------------------------------
 * @file: LXDict.h
 * @desc: Dictionary utility
 *
 * @history:
 * @+ <Gloup> : Aug 93 : first version
 * @+ <Gloup> : Jul 06 : Adapted to LXware
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * Helix dictionary library<br>
 * dictionnary data is a pointer to any user's defined structure. The
 * 'compare' function provided in the constructor is used to test data
 * equality (i.e. key equality) and to internally compare data. 
 * note: In this implementation Dictionary is backed up by a Red-Black tree,
 * provided by the generic red-black tree library by James S. Plank.
 *
 * @docend:
 */

#ifndef _H_LXDict

#define _H_LXDict

#ifndef _H_LXTypes
#include "LXTypes.h"
#endif

#ifndef _H_LXMacros
#include "LXMacros.h"
#endif

#ifndef _H_LXArray
#include "LXArray.h"
#endif

#ifndef NULL
#include <stdio.h>
#endif

/* ---------------------------------------------------- */
/* Structures                                           */
/* ---------------------------------------------------- */

/*
 * @doc: LXDict structure
 * - compare  : function to compare items : int compare(const void*, const void*)
 *              (same as in qsort(2))
 * - data     : - internal backup structure - do not alter -
 */

typedef struct {
    int       (*compare)(const void*, const void*); /* comparison function */
    void      *data;              /* internal structure                    */
} LXDict;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * create new empty dictionary
 * @param compare function function to compare userData :
 * compare(void* data1, void *data2) should return a positive, negative 
 * or null integer if data1>data2, data1<data2 or data1==data2 respectively
 * (usual convention). This wil be called intenrally by several API functions
 * such as LXHasDict, LXGetDict, LXRemoveDict.
 * @return new LXDict
 */

LXDict *LXNewDict(int (*compare)(const void*, const void*));

/*
 * @doc: 
 * free a whole dictionary
 * @param dict LXDict* dictionary to free
 * @param free user's function to free user's data.
 * may be NULL if you don't want to free data.
 * @return NULL
 */

LXDict *LXFreeDict (LXDict *dict, void (*free)(void*));

/*
 * @doc: 
 * tell if some user data is present in dictionary
 * @param dict LXDict* dictionary
 * @param userData void* some user data to search for.
 * note: the dictionary 'compare' function will be used
 * to compare data.
 * @return 1 if userData is present ; 0 if absent.
 */

int LXHasDict(LXDict *dict, void *userData);

/*
 * @doc: 
 * get user data from dictionary
 * @param dict LXDict* dictionary
 * @param userData void* some user data to search for.
 * note: the dictionary 'compare' function will be used
 * to retrieve data.
 * @return dictionary data if userData is present ; NULL if absent
 */

void *LXGetDict(LXDict *dict, void *userData);

/*
 * @doc: 
 * insert userData in dictionary
 * @param dict LXDict* dictionary
 * @param userData void* some user data to add.
 * note: the dictionary 'compare' function will be used
 * to check that data is not already present.
 * @return 1 if data has been added ; 0 else (i.e. if data is
 * already present).
 */

int LXAddDict(LXDict *dict, void *userData);

/*
 * @doc: 
 * remove some user data from dictionary
 * @param dict LXDict* dictionary
 * @param userData void* some user data to remove
 * @param free user's function to free user's data
 * may be NULL if you don't want to free data.
 * note: the dictionary 'compare' function will be used
 * to retrieve data. 
 * @return 1 if userData has been removed ; 0 else
 */

int LXRemoveDict(LXDict *dict, void *userData, 
                 void (*free)(void *userData));

/*
 * @doc: 
 * traverse a dictionary and take some action at each entry
 * @param dict LXDict* dictionary
 * @param action function function to call at each entry during traversal
 * this function is called as action(data, arg) where data is the current node data
 * and arg is any additional user's argument given as the last argument of 
 * LXTraverseDict. The action should not alter the current Dict structure
 * (i.e should not add or remove elements).
 * note: the traversal order is arbitrary (but deterministic)
 */

void LXTraverseDict(LXDict *dict,
                    void (*action)(void *data, void *arg),
                    void *arg);

/*
 * @doc: 
 * return an array of current entries. Each array value is a pointer
 * to user data as provided in LXAddDict. The array may therefore be corrupted
 * after a call to LXAddDict or LXRemoveDict. 
 * Note: element order is arbitrary, use LXSortArray to further sort array/
 * Note: this function will allocate a new array at each call, don't forget
 * to free array after use with LXFreeArray.
 * @param dict LXDict* dictionary
 * @return array of pointers
 */

LXArray *LXDictToArray(LXDict *dict);

#endif
