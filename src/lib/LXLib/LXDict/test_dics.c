/* test Dict */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXDict.h"

#define SIZE 100

/* ------------------------------------------------------------ */
/* test for dictionnary									        */
/* ------------------------------------------------------------ */

typedef struct {
   char *key;
   char *value;
} MyStruct;

void myStructFree(void *x) {
  MyStruct *s = (MyStruct *) x;
  FREE(s->key);
  FREE(s->value);
}

MyStruct *myStructNew(char *key, char *value) {
  MyStruct *s = NEW(MyStruct);
  s->key = NEWN(char, strlen(key)+1);
  (void) strcpy(s->key, key);
  s->value = NEWN(char, strlen(value)+1);
  (void) strcpy(s->value, value);
  return s;
}

int myStructCompar(const void *x, const void *y) {
  MyStruct * xx = (MyStruct *) x;
  MyStruct * yy = (MyStruct *) y;
  return strcmp(xx->key, yy->key);
}

void myAction(void *x, void *arg) {
/*ARGSUSED*/
  MyStruct * xx = (MyStruct *) x;
  printf("action key: %s value: %s\n", xx->key, xx->value);
}

void sPrintDict(LXDict *dict) {
  LXArray *arr = LXDictToArray(dict);
  Ptr *ptr = (Ptr *) arr->data;
  int i;
  for (i = 0 ; i < arr->nbelt ; i++) {
    MyStruct *x = (MyStruct *) ptr[i];
    printf("key: %s value: %s\n", x->key, x->value);
  }
  LXFreeArray(arr);
}

int main() {

  int i, k;
  LXDict *dict;
  
  dict = LXNewDict(myStructCompar);
  
  for (i = 0 ; i < SIZE ; i++) {

    char key[32], value[32];  
    
    (void) sprintf(key, "key-%5.5d", i);
    (void) sprintf(value, "value-%5.5d", i);
    
    MyStruct *s = myStructNew(key, value);
    
    (void) LXAddDict(dict, s);
  }

  LXTraverseDict(dict, myAction, NULL);
  
  sPrintDict(dict);
  
  LXFreeDict(dict, myStructFree);
  
  printf("test Dict OK\n");

  return 0;
}
