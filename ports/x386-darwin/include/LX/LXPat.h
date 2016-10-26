/* -----------------------------------------------------------------------
 * $Id: LXPat.h 1793 2013-02-24 02:19:06Z viari $
 * -----------------------------------------------------------------------
 * @file: LXPat.h
 * @desc: pattern scan
 * 
 * @history:
 * @+ <Gloup> : Dec 94 : first version
 * @+ <Gloup> : May 95 : revision
 * @+ <Wanou> : Jan 02 : adapted to LX_Ware
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LXpat library<br>
 * pattern scan library (Wu & Manber Algorithm)
 *
 * @docend:
 */

#ifndef _H_LXPat

#define _H_LXPat

#ifndef _H_LXStkp
#include "LXStkp.h"
#endif

/* ----------------------------------------------- */
/* constants                                       */
/* ----------------------------------------------- */

/*
 * @doc: End of data mark
 */
#define EOD                      -1

/*
 * @doc: max length of sequence name
 */
#define LX_PAT_MAX_NAME_LEN     512

/*
 * @doc: FIXED alphabet length (do not change !)
 */
#define LX_PAT_ALPHA_LEN         26

/*
 * @doc: max number of patterns
 */
#define LX_PAT_MAX_PATTERN       30

/*
 * @doc: max pattern length
 */

#define LX_PAT_MAX_PAT_LEN       32

/*
 * @doc: max number of errors
 */
#define LX_PAT_MAX_PAT_ERR       32

/*
 * @doc: FIXED mask for 26 symbols (do not change)
 */
#define LX_PAT_PATMASK          0x3ffffff       

/*
 * @doc: FIXED bit 27 to 1 -> oblig. pos (do not change)
 */
#define LX_PAT_OBLIBIT          0x4000000

/*
 * @doc: FIXED mask for highest position (do not change)
 */
#define LX_PAT_ONEMASK          0x80000000

/*
 * @doc: FIXED 26 letter alphabet in alphabetical order (do not change)
 */
#define LX_PAT_ORD_ALPHA        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

/*
 * @doc: special nomatch symbol 
 */
#define LX_PAT_NO_MATCH        'X'

/*
 * @docstart:
 * @par Pat Error codes
 * - LX_PAT_NO_ERROR           : no error
 * - LX_PAT_SYNTAX_ERROR       : pattern encoding error
 * @docend:
 */

#define LX_PAT_NO_ERROR         LX_NO_ERROR
#define LX_PAT_SYNTAX_ERROR     30

/* ----------------------------------------------- */
/* data structures                                 */
/* ----------------------------------------------- */

/*
 * @doc : Code Type
 * - alpha : standard [A-Z] alphabet
 * - dna : IUPAC DNA
 * - protein : IUPAC proteins
 * - proteinAlt : IUPAC alternative proteins (I=L; Q=K)
 */
                                        /* -------------------- */
typedef enum {                          /* data encoding        */
                                        /* -------------------- */
        ALPHA_TYPE = 0,                 /* [A-Z]                */
        DNA_TYPE,                       /* IUPAC DNA            */
        PROT_TYPE,                      /* IUPAC protein        */
        PROT_ALT_TYPE                   /* IUPAC protein I=L;Q=K*/
} LXPatCodeType;

/*
 * @doc: pattern structure
 * - patlen    : int     - pattern length
 * - maxerr    : int     - max number of errors
 * - cpat      : char[LX_PAT_MAX_PAT_LEN]   - pattern string
 * - patcode   : Int32[LX_PAT_MAX_PAT_LEN]  - encoded pattern
 * - smat      : UInt32[LX_PAT_ALPHA_LEN] - S matrix
 * - omask     : UInt32  - oblig. bits mask
 * - hasIndel  : Bool    - are indels allowed ? (TRUE/FALSE)
 * - ok        : Bool    - is pattern ok ? (TRUE/FALSE) 
 */
                                        /* -------------------- */
typedef struct {                        /* pattern              */
                                        /* -------------------- */
    int    patLen;                      /* pattern length       */
    int    maxErr;                      /* max # of errors      */
    char   cPat[LX_PAT_MAX_PAT_LEN+2];  /* pattern string       */
    Int32  patCode[LX_PAT_MAX_PAT_LEN]; /* encoded pattern      */
    UInt32 sMat[LX_PAT_ALPHA_LEN];      /* S matrix             */
    UInt32 oMask;                       /* oblig. bits mask     */
    Bool   hasIndel;                    /* are indels allowed   */
    Bool   ok;                          /* is pattern ok        */
} LXPattern;

/* ----------------------------------------------- */
/* prototypes                                      */
/* ----------------------------------------------- */

/*
 * @doc:
 * Return the correct (alpha or dna or protein) code
 * associated to code selector
 * @param codid LXPatCodeType the selected code type
 * @return pointer to code (Int32*)
 */
Int32 *LXPatGetCode (LXPatCodeType codid);

/*
 * @doc:
 * Encode pattern sequence
 * @param ppat LXPattern* pattern to encode sequence
 * @param cod LXPatCodeType code type selector
 * @return LX_PAT_NO_ERROR or LX_PAT_SYNTAX_ERROR
 * @sample
 *
 * <b>Example:</b>
 * <pre><code>
 * </code></pre>
 */
int LXPatEncodePattern (LXPattern *ppat, LXPatCodeType cod);

/*
 * @doc:
 * Encode (Fasta ?) sequence
 * @param seq char* sequence to encode
 * @return seq the encoded sequence 
 */
char  *LXPatEncodeSequence(char *seq);

/*
 * @doc:
 * Decode previously encoded sequence
 * @param data char* data to decode
 * @return data decoded data sequence
 */
char  *LXPatDecodeSequence(char *data);

/*
 * @doc:
 * Print Debug messages on stderr
 * @param ppat LXPattern* pattern to display
 */
void LXPatPrintDebugPattern (LXPattern *ppat);

/*
 * @doc:
 * Create S matrix for Manber algorithm
 * @param ppat LXPattern* pattern to modify
 * @return LX_PAT_NO_ERROR or LX_PAT_SYNTAX_ERROR
 */
int LXPatCreateS (LXPattern *ppat);

/*
 * @doc:
 * Manber pattern scan algorithm - No error
 * @param data char* encoded sequence to scan
 * @param ppat LXPattern* pattern to search
 * @param offset long start the search from here
 * @param length long data sequence length
 * @param stk LXStackp* stack to store match positions
 * @return error code or LX_NO_ERROR  
 */
int LXPatManberNoErr (char *data, LXPattern *ppat, long offset,
                      long length, LXStackp *stk);


/*
 * @doc:
 * Manber pattern scan algorithm - Substitutions only
 * @param data char* encoded sequence to scan
 * @param ppat LXPattern* pattern to search
 * @param offset long start the search from here
 * @param length long data sequence length
 * @param nbSub int max number of substitutions
 * @param stk LXStackp* stack to store match positions
 * @return error code or LX_NO_ERROR  
 */
int LXPatManberSubErr (char *data, LXPattern *ppat, long offset,
                       long length, int nbSub, LXStackp *stk);

/*
 * @doc:
 * Manber pattern scan algorithm - Substitutions + Indels
 * @param data char* encoded sequence to scan
 * @param ppat LXPattern* pattern to search
 * @param offset long start the search from here
 * @param length long data sequence length
 * @param nbErr int max number of substitutions + indels
 * @param stk LXStackp* stack to store match positions
 * @return error code or LX_NO_ERROR  
 */
int LXPatManberAllErr (char *data, LXPattern *ppat, long offset,
                       long length, int nbErr, LXStackp *stk);

/*
 * @doc:
 * Manber pattern scan algorithm - main entry point
 * use ppat->hasIndel and ppat->maxErr to decide which
 * version to call.
 * @param data char* encoded sequence to scan
 * @param ppat LXPattern* pattern to search
 * @param offset long start the search from here
 * @param length long data sequence length
 * @param stk LXStackp* stack to store match positions
 * @return error code or LX_NO_ERROR  
 */
int LXPatManber (char *data, LXPattern *ppat, long offset,
                 long length, LXStackp *stk);

#endif
