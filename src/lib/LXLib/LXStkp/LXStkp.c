/* -----------------------------------------------------------------------
 * $Id: LXStkp.c 1782 2013-02-23 00:11:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXStkp.c
 * @desc: dynamic stacks holding pointer - io functions
 * 
 * @history:
 * @+ <Gloup> : Mar 92 : first draft
 * @+ <Gloup> : Aug 93 : added xxxVector funcs
 * @+ <Wanou> : Dec 01 : adapted to LX_Ware 
 * @+ <Gloup> : Aug 06 : added direct access funcs
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LXStkp library <br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXSys.h"
#include "LX/LXStkp.h"

/* ============================ */
/* Constantes et Macros locales */
/* ============================ */

#define EXPAND_STACK(stkp) LXResizeStackp((stkp), (stkp)->size << 1)

#define SHRINK_STACK(stkp) LXResizeStackp((stkp), (stkp)->size >> 1)

static int sLastError = LX_NO_ERROR;

/* -------------------------------------------- */
/* local comparison function                    */
/* -------------------------------------------- */

static int sDefaultCompare(const void *s1, const void *s2) {
    return (int) ((const char *)s1 - (const char *)s2);
}


/* -------------------------------------------- */
/* gestion des erreurs                          */
/* get/reset erreur flag                        */
/*                                              */
/* @function: StkpError                         */
/* -------------------------------------------- */

int LXStkpError(Bool reset)
{
	int err;
	
	err = sLastError;
	
	if (reset)
	   sLastError = LX_NO_ERROR;
	   
	return err;
	
} /* end of StkpError */

/* -------------------------------------------- */
/* creation d'un stack                          */
/*                                              */
/* @function: LXNewStackp                       */
/* -------------------------------------------- */

LXStackpPtr LXNewStackp(size_t size)
{
	LXStackpPtr stkp;
	
	if (! (stkp = NEW(LXStackp))) {
		sLastError = LX_MEM_ERROR;
		return NULL;
    }

	stkp->size    = size;
	stkp->top     = 0;
	stkp->cursor  = 0;
	
	if ( ! (stkp->val = NEWN(Ptr, size))) {
		sLastError = LX_MEM_ERROR;
		return LXFreeStackp(stkp, NULL);
	}

	return stkp;    
					
} /* end of NewStackp */


/* -------------------------------------------- */
/* liberation d'un stack                        */
/* use destroy function to free internal        */
/* components                                   */
/*                                              */
/* @function: LXFreeStackp                      */
/* -------------------------------------------- */

LXStackpPtr LXFreeStackp(LXStackpPtr stkp, void (*destroy)(Ptr))
{
	if (stkp) {
		if (stkp->val) {
			if (destroy) {
				size_t i;
				for (i = 0 ; i < stkp->top ; i++)
					destroy(stkp->val[i]);
			}
			FREE(stkp->val);
		}
		FREE(stkp);
	}
	
	return NULL;
	
} /* end of FreeStackp */

/* -------------------------------------------- */
/* resize d'un stack                            */
/*                                              */
/* @function: ResizeStackp                      */
/* -------------------------------------------- */

size_t LXResizeStackp(LXStackpPtr stkp, size_t size)
{
	size_t resize = 0;  /* assume error         */
	Ptr    val;
			 
	if ((val = REALLOC(stkp->val, Ptr, size)) != NULL) {
		stkp->size = resize = size;
		stkp->val = val;
	}

	if (! resize)
		sLastError = LX_MEM_ERROR;

	return resize;
	
} /* end of ResizeStackp */

/* -------------------------------------------- */
/* copy stack                                   */
/*                                              */
/* @function: CopyStackp                        */
/* -------------------------------------------- */

LXStackpPtr LXCopyStackp (LXStackpPtr stkp, Ptr (*copy)(Ptr))
{
  size_t i;
  
  LXStackpPtr nstkp = LXNewStackp(stkp->top);
  
  if (! nstkp)
    return NULL;
    
  for (i = 0 ; i < stkp->top ; i++) {
    Ptr val = (copy ? copy(stkp->val[i]) : stkp->val[i]);
    (void) LXPushpIn(nstkp, val);
  }

  return nstkp;
}

/* -------------------------------------------- */
/* creation d'un vecteur de stacks              */
/*                                              */
/* @function: NewStackpVector                   */
/* -------------------------------------------- */

LXStackpHdle LXNewStackpVector(size_t vectSize, size_t stackSize)
{
	size_t i;
	LXStackpHdle stkh;
	
	if (! (stkh = NEWN(LXStackpPtr, vectSize))) {
		sLastError = LX_MEM_ERROR;
		return NULL;
	}
	
	for (i = 0 ; i < vectSize ; i++) {
		if (! (stkh[i] = LXNewStackp(stackSize)))
			return LXFreeStackpVector(stkh, i, NULL);
	}
		
	return stkh;
	
} /* end of NewStackpVector */


/* -------------------------------------------- */
/* liberation d'un vecteur de stacks            */
/*                                              */
/* @function: FreeStackpVector                  */
/* -------------------------------------------- */

LXStackpHdle LXFreeStackpVector(LXStackpHdle stkh, size_t vectSize,
                                void (*destroy)(Ptr))
{
	if (stkh) {
    	size_t i;
		for (i = 0 ; i < vectSize ; i++)
			(void) LXFreeStackp(stkh[i], destroy);
		FREE(stkh);
	}

	return NULL;
			
} /* end of FreeStackpVector */


/* -------------------------------------------- */
/* empilage(/lement)                            */
/*                                              */
/* @function: LXPushpIn                         */
/* -------------------------------------------- */

Bool LXPushpIn(LXStackpPtr stkp, Ptr val)
{
	if ((stkp->top >= stkp->size) && (! EXPAND_STACK(stkp)))
		return FALSE;

	stkp->val[(stkp->top)++] = val;

	return TRUE;
	
} /* end of PushpIn */

/* -------------------------------------------- */
/* depilage(/lement)                            */
/*                                              */
/* @function: LXPoppOut                         */
/* -------------------------------------------- */

Bool LXPoppOut(LXStackpPtr stkp, Ptr *val)
{
	if (stkp->top <= 0)
		return FALSE;
	
	(stkp->top)--;
	
	if (val) 
	  *val = stkp->val[stkp->top];

	if (    (stkp->top < (stkp->size >> 1)) 
		 && (stkp->top > LX_STKP_MIN_SIZE))
		(void) SHRINK_STACK(stkp);

	return TRUE;
	
} /* end of PoppOut */
                
/* -------------------------------------------- */
/* lecture descendante                          */
/*                                              */
/* @function: LXReadpDown                       */
/* -------------------------------------------- */

Bool LXReadpDown(LXStackpPtr stkp, Ptr *val)
{
	if (stkp->cursor == 0)
		return FALSE;
			
	*val = stkp->val[--(stkp->cursor)];
	
	return TRUE;
	
} /* end of ReadpDown */

/* -------------------------------------------- */
/* lecture ascendante                           */
/*                                              */
/* @function: LXReadpUp                         */
/* -------------------------------------------- */

Bool LXReadpUp(LXStackpPtr stkp, Ptr *val)
{
	if (stkp->cursor >= stkp->top)
		return FALSE;
			
	*val = stkp->val[(stkp->cursor)++];
	
	return TRUE;
	
} /* end of ReadpUp */

/* -------------------------------------------- */
/* remontee/descente du curseur                 */
/*                                              */
/* @function: LXCurspToTop                      */
/* @function: LXCurspToBottom                   */
/* -------------------------------------------- */

void LXCurspToTop(LXStackpPtr stkp)
{
	stkp->cursor = stkp->top;

} /* end of CurspToTop */

void LXCurspToBottom(LXStackpPtr stkp)
{
	stkp->cursor = 0;

} /* end of CurspToBottom */

/* -------------------------------------------- */
/* echange des valeurs cursor <-> (top - 1)     */
/*                                              */
/* @function: LXCurspSwap                       */
/* -------------------------------------------- */

void LXCurspSwap(LXStackpPtr stkp)
{
	Ptr tmp;
	
	if (stkp->top == 0)
		return;
	
	tmp = stkp->val[stkp->cursor];
	stkp->val[stkp->cursor] = stkp->val[stkp->top - 1];
	stkp->val[stkp->top - 1] = tmp;
	
} /* end of CurspSwap */



/* -------------------------------------------- */
/* get current cursor position                  */
/*                                              */
/* @function: LXCurspGetPos                     */
/* -------------------------------------------- */

int LXCurspGetPos (LXStackpPtr stkp)
{
	return stkp->cursor - 1;
}

/* -------------------------------------------- */
/* get value at current cursor position         */
/*                                              */
/* @function: LXCurspGetValue                   */
/* -------------------------------------------- */

Ptr LXCurspGetValue (LXStackpPtr stkp)
{
	if ((stkp->cursor <= 0) || (stkp->cursor > stkp->top))
		return NULL;
		
	return stkp->val[stkp->cursor - 1];
}

/* -------------------------------------------- */
/* Recherche d'une valeur en stack a partir du  */
/* curseur courant en descendant (O(n))         */
/* on laisse le curseur a l'endroit trouve      */
/*                                              */
/* @function: LXSearchDownStackp                */
/* -------------------------------------------- */

Bool LXSearchDownStackp(LXStackpPtr stkp, Ptr sval, 
                       int (*compar)(const void *, const void*))
{
	Ptr     val;
	Bool    more;
	
	if (! compar)
	  compar = sDefaultCompare;
	
	while ((more = LXReadpDown(stkp, &val))) {
		if (compar(val, sval) == 0)
			break;
	}
	
	return more;
	
} /* end of SearchDownStackp */

/* -------------------------------------------- */
/* Recherche dichotomique d'une valeur en stack */
/* le stack est suppose trie par valeurs        */
/* croissantes.                                 */
/* on place le curseur a l'endroit trouve       */
/*                                              */
/* @function: LXBinSearchStackp                 */
/* -------------------------------------------- */

Bool LXBinSearchStackp(LXStackpPtr stkp, Ptr sval, 
                       int (*compar)(const void *, const void*))
{
  long midd, low, high, span;

  low  = 0;
  high = stkp->top - 1;
  
  while (high >= low) {   

    midd = (high + low) / 2;

    span = compar(stkp->val[midd], sval);

    if (span == 0) {
      stkp->cursor = midd;
      return TRUE;
    }

    if (span > 0)
      high = midd - 1;
    else
      low  = midd + 1;
  }

  return FALSE;
        
} /* end of BinSearchStacki */

/* -------------------------------------------- */
/* tri un stack                                 */
/*                                              */
/* @function: LXSortStackp                      */
/* -------------------------------------------- */

void LXSortStackp (LXStackpPtr stkp, int (*compar)(const void *, const void*))
{
	qsort(stkp->val, stkp->top, sizeof(Ptr), compar);
}

/* -------------------------------------------- */
/* inverse l'ordre des elements dans un stack   */
/*                                              */
/* @function: LXReverseStackp                   */
/* -------------------------------------------- */

void LXReverseStackp(LXStackpPtr stkp)
{
	Ptr *t, *b, swp;

	b = stkp->val;
	t = b + stkp->top - 1;

	while (t > b) {
		 swp  = *t;
		 *t-- = *b;
		 *b++ = swp;
	}
	
} /* end of ReverseStackp */

/* -------------------------------------------- */
/* taille stack                                 */ 
/* -------------------------------------------- */

size_t LXStackpSize(LXStackpPtr stkp)
{
	return stkp->top;
}

/* -------------------------------------------- */
/* taille stack                                 */ 
/* -------------------------------------------- */

Bool LXIsStackpEmpty (LXStackpPtr stkp)
{
	return (stkp->top <= 0);
}

/* -------------------------------------------- */
/* acces a l'element d'index indx               */
/* -------------------------------------------- */

Ptr LXStackpGetValue(LXStackpPtr stkp, size_t indx)
{
    return (indx < stkp->top ? stkp->val[indx] : NULL);
}

/* -------------------------------------------- */
/* acces a l'element top                        */
/* -------------------------------------------- */

Ptr LXStackpGetTop(LXStackpPtr stkp)
{
    if (stkp->top == 0)
        return NULL;

    return stkp->val[stkp->top-1];
}
