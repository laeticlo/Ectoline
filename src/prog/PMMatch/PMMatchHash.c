/* -----------------------------------------------------------------------
 * $Id: PMMatchHash.c 993 2010-04-14 22:51:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMMatchHash.c
 * @desc: PMMatch hashing functions 
 *
 * @history:
 * @+ <Wanou> : Jul 02 : first version
 * @+ <Gloup> : Aug 06 : complete code revision
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PepMap PMChromo hashing functions<br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX.h"
#include "PM.h"

#include "PMMatch.h"

#define ALPHALEN 26  /* do not change */

/* cache powers of ALPHALEN */

static int sPow[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* ----------------------------------------------- */
/* local functions                                 */
/* ----------------------------------------------- */

/* ----------------------------------------------- */
/* compute ALPHALEN^k                              */
/* ----------------------------------------------- */

static int sPower(int k) {

  if (sPow[k] != 0)
    return sPow[k];

  if (sPow[k] == 0) {
    int i, pow;
    for (i = 0, pow = 1 ; i < k ; i++) {
      pow *= ALPHALEN;
    }
    sPow[k] = pow;
  }

  return sPow[k];
}

/* ----------------------------------------------- */
/* return correct symbol to use with               */
/* undifferentiable symbol                         */
/* ----------------------------------------------- */

static int sDifferentiate(int symbol, Bool differentiateKQ)
{
  if (symbol == 'L')
    return 'I';
  if ((symbol == 'Q') && (! differentiateKQ))
    return 'K';
  return symbol;
}

/* ----------------------------------------------- */
/* empty stack                                     */
/* ----------------------------------------------- */

static LXStackpPtr sEmptyStack(LXStackpPtr *stkh)
{
  Ptr val;

#if 0

  /* ---------------------------- */
  /* proper way                   */  

  while (LXPoppOut(stkh, &val))
     /* nop */;

#else

  /* ---------------------------- */
  /* ugly way but more efficient  */

  (*stkh)->top = 0;

#endif
  
  return *stkh;
}

/* ----------------------------------------------- */
/* API                                             */
/* ----------------------------------------------- */

/* ----------------------------------------------- */
/* compute hashcode of k-uple                      */
/* ----------------------------------------------- */

Int32 HashUple(char *s, int kuple, Bool differentiateKQ)
{
  int   i, symb;
  Int32 code = 0;
  
  for (i = 0 ; i < kuple ; i++) {

    code *= ALPHALEN;

    symb = sDifferentiate(s[i], differentiateKQ);

    code += (symb - 'A');
  }
  
  return code;
}

/* ----------------------------------------------- */
/* sliding hashcode                                */
/* ----------------------------------------------- */

Int32 HashNextUple(char *s, int kuple, Bool differentiateKQ, Int32 previous)
{
  int   symb;
  Int32 power, code;
  
  power = sPower(kuple - 1);
  
  code  = (previous - (((Int32) (previous / power)) * power));
  
  code *= ALPHALEN;

  symb  = sDifferentiate(s[kuple-1], differentiateKQ);
  
  code += (symb - 'A');
  
  return code;
}

/* ----------------------------------------------- */
/* new HashTable                                   */
/* ----------------------------------------------- */

HashTable *HashNewTable(int kuple)
{
  int i;
  HashTable *table;

  if (! (table = NEW(HashTable))) {
     MEMORY_ERROR();
     return NULL;
  }

  table->kuple = kuple;
  table->size  = sPower(kuple);
  
   if (! (table->stk = NEWN(LXStackpPtr, table->size))) {
     MEMORY_ERROR();
     return HashFreeTable(table);
   }
     
   for (i = 0 ; i < table->size ; i++) {
     table->stk[i] = NULL;
   }
   
   return table;
}

/* ----------------------------------------------- */
/* free HashTable                                  */
/* ----------------------------------------------- */

HashTable *HashFreeTable(HashTable *table)
{
  int i;
  
  if (table) {
    if (table->stk)
      HashResetTable(table);
    FREE(table);
  }
  return NULL;
}

/* ----------------------------------------------- */
/* reset HashTable to null entries                 */
/* ----------------------------------------------- */

void HashResetTable(HashTable *table)
{
  int i;
  
  for (i = 0 ; i < table->size ; i++) {
    if (table->stk[i])
      table->stk[i] = LXFreeStackp(table->stk[i], NULL);
  }
}

/* ----------------------------------------------- */
/* reset HashTable by flattening entries           */
/* ----------------------------------------------- */

void HashFlattenTable(HashTable *table)
{
  int i;
  
  for (i = 0 ; i < table->size ; i++) {
    if (table->stk[i])
      table->stk[i] = sEmptyStack(table->stk + i);
  }
}

/* ----------------------------------------------- */
/* initialize HashTable entries according to tags  */
/* ----------------------------------------------- */

void HashInitTable(HashTable *table, PMTagStack *tagStack, Bool differentiateKQ)
{
  long i;
  
  for (i = 0 ; i < tagStack->top ; i++) {
  
    PMTag *tag = (PMTag *) tagStack->val[i];
    
    (void) UtilValidateSequence(tag->seq);
    
    Int32 code = HashUple(tag->seq, strlen(tag->seq), differentiateKQ);
    
    if (! table->stk[code])
      table->stk[code] = LXNewStackp(LX_STKP_MIN_SIZE);
  }
}

/* ----------------------------------------------- */
/* Fill HashTable entries with sequence positions  */
/* ----------------------------------------------- */

void HashFillTable(HashTable *table, char *seq, int kuple, Bool differentiateKQ)
{
  Int32 pos, code, previous;
  char  *s, *end;
    
   /* ---------------------------------- */
   /* check if sequence is large enough  */

   for (pos = 1, end = seq; pos < kuple ; pos++, end++) {
     if (! *end++)
       return;
   }
   
   /* ---------------------------------- */
   /* encode first kuple                 */
      
   code = previous = HashUple(seq, kuple, differentiateKQ);

   if (table->stk[code])
     (void) LXPushpIn(table->stk[code], (void *) 0);

   /* ---------------------------------- */
   /* now go ahead, 'end' will ensure    */
   /* we stop kuple positions from end   */

   for (s = seq + 1, pos = 1 ; *end ; s++, end++, pos++) {
    
     code = HashNextUple(s, kuple, differentiateKQ, previous);

     if (table->stk[code])
       (void) LXPushpIn(table->stk[code], (void *) pos);
      
     previous = code;
   }
}

/* ----------------------------------------------- */
/* Lookup for tag hits in hashTable                */
/* return stack of hits (may be empty)             */
/* ----------------------------------------------- */

PMHitStack *HashLookupHits(HashTable *table, PMTag *tag, Bool differentiateKQ)
{
  long       i;
  Int32      code;
  PMHitStack *hitStack;

  if (! (hitStack = PMNewHitStack())) {
    MEMORY_ERROR();
    return NULL;
  }
      
  code = HashUple(tag->seq, table->kuple, differentiateKQ);
  
  if (! table->stk[code])
    return hitStack;
    
  for (i = 0 ; i < table->stk[code]->top ; i++) {

    PMHit *hit;

    if (! (hit = PMNewHit())) {
      MEMORY_ERROR();
      return NULL;
    }

    hit->type  = InvalidHit;
    hit->posSn = (int) table->stk[code]->val[i];
    hit->posSc = hit->posSn + table->kuple - 1;
    hit->tag   = tag;
    
    (void) LXPushpIn(hitStack, (void *) hit);
  }

  return hitStack;
}

