/* -----------------------------------------------------------------------
 * $Id: LXPat.c 1782 2013-02-23 00:11:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXPat.c
 * @desc: pattern scan IO and functs
 * 
 * @history:
 * @+ <Gloup> : Dec 94 : first version
 * @+ <Gloup> : May 95 : revision
 * @+ <Wanou> : Jan 02 : adapted to LX_Ware
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LXPat C program<br>
 *
 * @docend:
 */

 
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX.h"

                                /* -------------------- */
                                /* default char         */
                                /* encodings            */
                                /* -------------------- */
 
static /*const*/ Int32 sDftCode[] = {
#include "LX/CODES/dft_code.h"
};
 
                                /* -------------------- */
                                /* char encodings       */
                                /* IUPAC                */
                                /* -------------------- */
 
                                /* IUPAC Proteins       */
static /*const*/ Int32 sProtCode[] = {
#include "LX/CODES/prot_code.h"
};
 
static /*const*/ Int32 sProtAltCode[] = {
#include "LX/CODES/dft_code_alt.h"
};


                                /* IUPAC Dna/Rna        */
static /*const*/ Int32 sDnaCode[] = {
#include "LX/CODES/dna_code.h"
};

/* ------------------------------------ */

Int32 *LXPatGetCode (LXPatCodeType codid)
{
  switch(codid) {
    case ALPHA_TYPE :
        return sDftCode;
        break;
    case DNA_TYPE :
        return sDnaCode;
        break;
    case PROT_TYPE :
        return sProtCode;
        break;
    case PROT_ALT_TYPE : 
        return sProtAltCode;
        break;
    }

    return NULL; /* no such code */
}

/* ------------------------------------ */

int LXPatEncodePattern (LXPattern *ppat, LXPatCodeType codid)
{
  int i;
  Int32 *code;

  code = LXPatGetCode(codid);
  ppat->ok = FALSE;

  for (i = 0; i < ppat->patLen ; i++) {

    int c = ppat->cPat[i];
    
    if (! isalpha(c)) {
      return LX_PAT_SYNTAX_ERROR;
    }

    if (islower(c)) {
      c = toupper(c);
    }

    ppat->patCode[i] = code[c - 'A'];
  }

  ppat->ok = TRUE;

  return LX_PAT_NO_ERROR;
}

/* ------------------------------------ */

int LXPatCreateS (LXPattern *ppat)
{
  int i;
  unsigned int amask;

  ppat->ok = FALSE;

  ppat->oMask = 0x0L;

  for (i = 0; i < LX_PAT_ALPHA_LEN; i++) {
      ppat->sMat[i] = 0x0;
  }

  for (i = ppat->patLen - 1, amask = 0x1L ; i >= 0 ; i--, amask <<= 1) {

      int indx, j;
      unsigned int pindx;

      indx = ppat->patCode[i];
 
      if (indx & LX_PAT_OBLIBIT) {
        ppat->oMask |= amask;
      }
 
      for (j = 0, pindx = 0x1L ; j < LX_PAT_ALPHA_LEN ; j++, pindx <<= 1) {
        if (indx & pindx)
          ppat->sMat[j] |= amask;
      }
  }

  ppat->ok = TRUE;

  return LX_PAT_NO_ERROR;
}

/* ------------------------------------ */

char *LXPatEncodeSequence(char *seq)
{
  char *s;
  
  for (s = seq ; *s ; s++) {
  
    int c = *s;
    
    if (! isalpha(c))
      c = LX_PAT_NO_MATCH;  // what should we do here ?
      
    if (islower(c))
      c = toupper(c);

    *s = c - 'A';
  }
  
  *s = EOD;

  return seq;
}

/* ------------------------------------ */

char *LXPatDecodeSequence(char *data)
{
  char *s;
  
  for (s = data ; *s != EOD ; s++)
    *s += 'A';

  *s = '\000';
  
  return data;
}

/* ------------------------------------ */

void LXPatPrintDebugPattern (LXPattern *ppat)
{
  int i;

  char alpha[LX_PAT_ALPHA_LEN+1];

  (void) strcpy(alpha, LX_PAT_ORD_ALPHA);

  (void) fprintf(stderr,"patCode:\n");
  for (i = 0 ; i < ppat->patLen ; i++)
      fprintf(stderr, "%d ", ppat->patCode[i]);
  (void) fprintf(stderr, "\n");

  (void) fprintf(stderr,"sMat:\n");
  for (i = 0 ; i < LX_PAT_ALPHA_LEN ; i++) {
      if (ppat->sMat[i])
          (void) fprintf(stderr, "%c : 0x%x\n", alpha[i], ppat->sMat[i]);
  }
}

/* ------------------------------------ */

int LXPatManberNoErr (char *data, LXPattern *ppat, long offset,
                      long length, LXStackp *stk)
{
  long    pos;
  UInt32  smask, r;
  char    *sdata;
                                /* create local masks   */

  smask = r = 0x1L << ppat->patLen;

  sdata = data + offset;        /* shift data           */
        
                                /* loop on text data    */
                                        
  for (pos = offset ; pos < length ; pos++) {

    r = (r >> 1) & ppat->sMat[(int) *sdata++];

    if (     (r & 0x1L)
         &&  (! LXPushpIn(stk, (Ptr)(pos-ppat->patLen+1)))) {
      return MEMORY_ERROR();
    }

    r |= smask;
  }
        
  return LX_NO_ERROR;
}

/* ------------------------------------ */

Int32 LXPatManberSubErr (char *data, LXPattern *ppat, long offset,
                         long length, int nbSub, LXStackp *stk)
{
  long pos;
  int  e;
  UInt32 smask, cmask, *pr, r[2*LX_PAT_MAX_PAT_ERR+2];
  char *sdata;

  r[0] = r[1] = 0x0;

  cmask = smask = 0x1L << ppat->patLen;

  for (e = 0, pr = r + 3 ; e <= nbSub ; e++, pr += 2)
    *pr = cmask;

  cmask = ~ ppat->oMask;

  sdata = data + offset;
  
  for (pos = offset ; pos < length ; pos++) {

    int found = 0;
    
    UInt32 sindx = ppat->sMat[(int) *data++];

    for (e = 0, pr = r ; e <= nbSub ; e++, pr += 2) {
    
      pr[2]  = pr[3] | smask;

      pr[3]  =   ((pr[0] >> 1) & cmask)    /* sub   */
               | ((pr[2] >> 1) & sindx);   /* ident */
               
      if (found)
        continue;  /* position already registered */
      
      if (pr[3] & 0x1L) {  /* found */
      
        if (! LXPushpIn(stk, (Ptr)(pos-ppat->patLen+1)))
          return MEMORY_ERROR();

        found++;
      }
    }
  }  

  return LX_NO_ERROR;
}

/* ------------------------------------ */

Int32 LXPatManberAllErr (char *data, LXPattern *ppat, long offset,
                         long length, int nbErr, LXStackp *stk)
{
  long pos;
  int e;
  UInt32 smask, cmask, *pr, r[2*LX_PAT_MAX_PAT_ERR+2];
  char *sdata;

  r[0] = r[1] = 0x0;
  cmask = smask = 0x1L << ppat->patLen;

  for (e = 0, pr = r + 3 ; e <= nbErr ; e++, pr += 2) {
    *pr = cmask;
    cmask = (cmask >> 1) | smask;
  }

  cmask = ~ ppat->oMask;

  sdata = data + offset;

  for (pos = offset ; pos < length ; pos++) {

    int found = 0;
  
    UInt32 sindx = ppat->sMat[(int) *sdata++];

    for (e = 0, pr = r ; e <= nbErr ; e++, pr += 2) {
    
      pr[2] = pr[3] | smask;
        
      pr[3] =  (  (  (     pr[0]           /* ins   */
                       |  (pr[0] >> 1)     /* sub   */
                       |  (pr[1] >> 1))    /* del   */
                    & cmask)
                | ((pr[2] >> 1) & sindx)); /* ident */

      if (found)
        continue;  /* position already registered */
      
      if (pr[3] & 0x1L) {  /* found */
      
        if (! LXPushpIn(stk, (Ptr)(pos-ppat->patLen+1)))
          return MEMORY_ERROR();

        found++;
      }
    }
  }

  return LX_NO_ERROR;
}

/* ------------------------------------ */

Int32 LXPatManber (char *data, LXPattern *ppat, long offset,
                   long length, LXStackp *stk)
{
  if(ppat->hasIndel && ppat->maxErr>=0)
    return(LXPatManberAllErr(data, ppat, offset, length, ppat->maxErr, stk));
    
  if(!ppat->hasIndel && ppat->maxErr>=0)
    return(LXPatManberSubErr(data, ppat, offset, length, ppat->maxErr, stk));
    
  return(LXPatManberNoErr(data, ppat, offset, length, stk));
}
