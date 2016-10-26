/* -----------------------------------------------------------------------
 * $Id: LXDict.c 1782 2013-02-23 00:11:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXDict.h
 * @desc: Helix dictionary  utility
 * 
 * @history:
 * @+ <Gloup> : Aug 93 : first version
 * @+ <Gloup> : Jul 06 : Adapted to LXware
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * Helix dictionary  library<br>
 * In this implementation the dictionary is backed up by a Red-Black Tree
 * structure that guarantees log(n) access time.
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXSys.h"
#include "LX/LXDict.h"

#include "LXrb.h"

typedef int (*treeCompare)(char *, char *);

/*----------------------------------------------------- */
/* free a whole dictionary                              */
/*----------------------------------------------------- */

LXDict *LXFreeDict(LXDict *dict, void (*destroy)(void*)) {

  IFF_RETURN(dict, NULL);

  if (dict->data) {
    Rb_node tree, n;
    tree = (Rb_node) dict->data;
    n = rb_first(tree);
    while (n != rb_nil(tree)) {
      Rb_node m = rb_next(n);
      if (destroy) {
        destroy(n->k.key);
      }
      FREE(n);
      n = m;
    }
  }
  
  FREE(dict);
  
  return NULL;
}

/*----------------------------------------------------- */
/* create new empty dictionary                          */
/*----------------------------------------------------- */

LXDict *LXNewDict(int (*compare)(const void*, const void*)) {

   LXDict *dict = NEW(LXDict);
   IFF_RETURN(dict, NULL);

   dict->compare = compare;   
   
   dict->data = make_rb();
   
   IFF_RETURN(dict, LXFreeDict(dict, NULL));

   return dict;
}

/*----------------------------------------------------- */
/* tell if some user data is present in dictionary      */
/*----------------------------------------------------- */

int LXHasDict(LXDict *dict, void *userData) {

  IFF_RETURN(dict, 0);
  
  int found = 0;
  (void) rb_find_gkey_n((Rb_node) dict->data, (char *) userData,
                        (treeCompare) dict->compare, &found);
  return (found ? 1 : 0);
}

/*----------------------------------------------------- */
/* get user data from dictionary                        */
/*----------------------------------------------------- */

void *LXGetDict(LXDict *dict, void *userData) {

  IFF_RETURN(dict, NULL);
  
  int found = 0;
  Rb_node n = rb_find_gkey_n((Rb_node) dict->data, (char *) userData,
                              (treeCompare) dict->compare, &found);
  return (found ? n->k.key : NULL);
}

/*----------------------------------------------------- */
/* insert userData in dictionary                        */
/*----------------------------------------------------- */

int LXAddDict(LXDict *dict, void *userData) {

  IFF_RETURN(dict, 0);
  
  if (LXHasDict(dict, userData))
    return 0;
    
  (void) rb_insertg((Rb_node) dict->data, (char *) userData, NULL,
                    (treeCompare) dict->compare);
  return 1;
}

/*----------------------------------------------------- */
/* remove some user data from dictionary                */
/*----------------------------------------------------- */

int LXRemoveDict(LXDict *dict, void *userData, 
                 void (*destroy)(void *userData)) {

  IFF_RETURN(dict, 0);
  
  int found = 0, removed = 0;
  Rb_node node;
  
  // [TODO] remove while 
  while ((node = rb_find_gkey_n((Rb_node) dict->data, (char *) userData,
                 (treeCompare) dict->compare, &found)) != NULL) {
    if (! found)
      break;
      
     removed = 1;
     
     if (destroy)
       destroy(node->k.key);
       
     rb_delete_node(node);
  }

  return removed;  
}


/*----------------------------------------------------- */
/* traverse dictionary tree                             */
/*----------------------------------------------------- */

void LXTraverseDict(LXDict *dict, 
                   void (*action)(void *data, void *arg),
                   void *arg) {

  if (! dict) return;
  
  Rb_node tree, n;
  tree = (Rb_node) dict->data;
  rb_traverse(n, tree) {
    action(n->k.key, arg);
  }
}

/*----------------------------------------------------- */
/* get array of bodies                                  */
/*----------------------------------------------------- */

//
// traversal utility
//
static void sStore(void *data, void *arg) {
  (void) LXAppendArray((LXArray *) arg, &data, 1);
}

//
// API
//
LXArray *LXDictToArray(LXDict *dict) {

  IFF_RETURN(dict, NULL);
    
  LXArray *arr = LXNewArray(sizeof(void*), 0);

  if (arr) {
    LXTraverseDict(dict, sStore, arr);
  }
    
  return arr;
}
