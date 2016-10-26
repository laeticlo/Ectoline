/* -----------------------------------------------------------------------
 * $Id: LXBio.c 1782 2013-02-23 00:11:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXBio.c
 * @desc: bio sequences utility functions
 * 
 * @history:
 * @+ <Gloup> : Jan 96 : first version for PWG 
 * @+ <Wanou> : Jan 02 : adapted to LX_Ware 
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * Genetic sequences library <br>
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* to instanciate genetic codes at this single place */

#define _LX_GENETIC_CODE_INSTANCE_

#include "LX/LXBio.h"

static char sNuc[]     = LX_BIO_DNA_ALPHA;
static char sAnuc[]    = LX_BIO_CDNA_ALPHA;
static char sGenNuc[]  = LX_GENETIC_NUC_ALPHA;
static char sGenPro[]  = LX_GENETIC_PRO_ALPHA;
static int  sNucNum[5] = {0, 1, 2, 3, 3};

/* ---------------------------- */

char LXBioBaseComplement(char nucAc)
{
    char *c;

    if ((c = strchr(sNuc, nucAc)) != 0)
        return sAnuc[(c - sNuc)];
    else
        return nucAc;
}

/* ---------------------------- */

char *LXBioSeqComplement(char *nucAcSeq)
{
    char *s;

    for (s = nucAcSeq ; *s ; s++)
        *s = LXBioBaseComplement(*s);

    return nucAcSeq;
}

char *LXBioSeqReverseComplement(char *nucAcSeq)
{
    return LXReverseStr(LXBioSeqComplement(nucAcSeq));
}

/* ---------------------------- */

static char sCodonTranslate(char *codon, int *code)
{
    int  i, base, hash = 0;
    char *p;
    
    for (i = 0 ; i < LX_BIO_CODON_LENGTH ; i++) {
        if ((p = strchr(sGenNuc, codon[i])) != NULL) {
            base = ((int) (p - sGenNuc)) / 2;
            hash = (hash * 4) + sNucNum[base];
        }
        else {
            hash = 64;  /* bad letter in codon  */
            break;      /* or incomplete codon  */
        }
    }

    return sGenPro[code[hash]];
}

/* ---------------------------- */

char LXBioCodonTranslate(char codon[LX_BIO_CODON_LENGTH], int codid)
{
    if ((codid < 0) || (codid >= LX_GENETIC_MAX_CODES))
        return LX_GENETIC_BAD_AA;

    return sCodonTranslate(codon, gLXGeneticCode[codid].code);
}

/* ---------------------------- */

char *LXBioSeqTranslate(char *nucAcSeq, int codid)
{
    int  *code;
    char *ps, *ns;
    
    if ((codid < 0) || (codid >= LX_GENETIC_MAX_CODES))
        return NULL;

    code = gLXGeneticCode[codid].code;

    for (ns = ps = nucAcSeq ; ns[0] && ns[1] && ns[2] ; ns += 3)
        *ps++ = sCodonTranslate(ns, code);
    
    *ps = '\000';

    return nucAcSeq;    
} 

/* ---------------------------- */

int LXBioSeqType(char *seq)
{
    char  *s;
    int   count = 0, sum = 1;
    float frac;

    for (s = seq ; *s ; s++) {
    
      int symb = toupper(*s);
      
      if ((symb == 'N') || (symb == 'X'))
        continue;
      
      if ((symb == 'C') || (symb == 'T'))
        count++;
        
      if (++sum > 1000) // 1000 symbols are enough
        break;
    }

    if (sum < 100)
      return LX_BIO_SEQ_UNKNOWN;
      
    frac = (float) count / (float) sum;
    
    if (frac > 0.2)
      return LX_BIO_SEQ_DNA;
      
    return LX_BIO_SEQ_PROTEIN;
}
