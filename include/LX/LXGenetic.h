/* -----------------------------------------------------------------------
 * $Id: LXGenetic.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: LXGenetic.h
 * @desc: Genetic codes / include file
 * 
 * @history:
 * @+ <Gloup> : Jan 96 : first version for PWG from acnuc
 * @+ <Wanou> : Jan 02 : adapted to LX_Ware 
 *                       (X = stop codon translation; Z = error)
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * Genetic codes / include file<br>
 *
 * @docend:
 */

#ifndef _H_LXGenetic

#define _H_LXGenetic

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @doc: nucleic acids
 */

#define LX_GENETIC_NUC_ALPHA    "AaCcGgTtUu"

/*
 * @doc: amino acids (X = stop codon; Z = error)
 */
#define LX_GENETIC_PRO_ALPHA    "RLSTPAGVKNQHEDYCFIMWXZ"

/*
 * @doc: unknown amino acid (Z)
 */
#define LX_GENETIC_BAD_AA       'Z'

/*
 * @doc: Stop codon amino acid (X)
 */
#define LX_GENETIC_STOP_AA      'X'

/*
 * @doc: number of genetic codes
 */
#define LX_GENETIC_MAX_CODES    9

/*
 * @doc: universal code
 */
#define LX_GENETIC_CODE_UNIVL  0   /* Universal                 */

/*
 * @doc: mito yeast code
 */
#define LX_GENETIC_CODE_MYEAS  1   /* mito yeast                */

/*
 * @doc: mito vertebrate code
 */
#define LX_GENETIC_CODE_MVERT  2   /* mito vertebrate           */

/*
 * @doc: filamentous fungi code
 */
#define LX_GENETIC_CODE_FUNGI  3   /* filamentous fungi         */

/*
 * @doc: mito insects & platyhelminthes code
 */
#define LX_GENETIC_CODE_MINSE  4   /* mito insects & platyhelminthes */

/*
 * @doc: Candida cylindracea code
 */
#define LX_GENETIC_CODE_CANDI  5   /* Candida cylindracea       */

/*
 * @doc: Ciliata code
 */
#define LX_GENETIC_CODE_CILIA  6   /* Ciliata                   */

/*
 * @doc: Euplotes code
 */
#define LX_GENETIC_CODE_EUPLO  7   /* Euplotes                  */

/*
 * @doc: mito echinoderms code
 */
#define LX_GENETIC_CODE_MECHI  8   /* mito echinoderms          */

/*
 * @doc: number of codons
 */
#define LX_GENETIC_MAX_CODONS     64

/*
 * @doc: maximum string length
 */
#define LX_GENETIC_MAX_STRING_LEN  256

/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/*
 * @doc: genetic code structure
 * - title    : char[LX_GENETIC_MAX_STRING_LEN] - genetic code name
 * - info     : char[LX_GENETIC_MAX_STRING_LEN] - genetic code infos
 * - code     : int[LX_GENETIC_MAX_CODONS + 1] - 64 codons + 1 error
 */

typedef struct LXGeneticCode {
    char   title[LX_GENETIC_MAX_STRING_LEN+2];  /* nom du code          */
    char   info[LX_GENETIC_MAX_STRING_LEN+2];   /* informations         */
    int    code[LX_GENETIC_MAX_CODONS + 1];     /* 64 codons + 1 Error  */
} LXGeneticCode;

/* ---------------------------------------------------- */
/* Global Data Declaration                              */
/* ---------------------------------------------------- */

extern LXGeneticCode gLXGeneticCode[LX_GENETIC_MAX_CODES];

/* ---------------------------------------------------- */
/* Global Data Instanciation                            */
/* ---------------------------------------------------- */

#ifdef _LX_GENETIC_CODE_INSTANCE_

    /* define _LX_GENETIC_CODE_INSTANCE_ in your code
     * at the (unique) place where you want to
     * instanciate the genetic codes table.
     */

/*
 * @doc: "the" genetic code - global variable -
 */

LXGeneticCode gLXGeneticCode[LX_GENETIC_MAX_CODES] = {

{ /* 0: UNIVERSAL */
        {"Universal"},
        {""},
        { 8,  9,  8,  9,  3,  3,  3,  3, 
          0,  2,  0,  2, 17, 17, 18, 17,
         10, 11, 10, 11,  4,  4,  4,  4, 
          0,  0,  0,  0,  1,  1,  1,  1, 
         12, 13, 12, 13,  5,  5,  5,  5, 
          6,  6,  6,  6,  7,  7,  7,  7, 
         20, 14, 20, 14,  2,  2,  2,  2, 
         20, 15, 19, 15,  1, 16,  1, 16, 
         21}
}, 

{ /* 1: MITOCHONDRIAL YEAST */
        {"Mitochondrial Yeast"}, 
        {"CUN=T  AUA=M  UGA=W"},
        { 8,  9,  8,  9,  3,  3,  3,  3, 
          0,  2,  0,  2, 18, 17, 18, 17,
         10, 11, 10, 11,  4,  4,  4,  4, 
          0,  0,  0,  0,  3,  3,  3,  3, 
         12, 13, 12, 13,  5,  5,  5,  5, 
          6,  6,  6,  6,  7,  7,  7,  7, 
         20, 14, 20, 14,  2,  2,  2,  2, 
         19, 15, 19, 15,  1, 16,  1, 16, 
         21}
}, 

{ /* 2: MITOCHONDRIAL VERTEBRATES */
        {"Mitochondrial Vertebrates"}, 
        {"AGR=*  AUA=M  UGA=W"},
        { 8,  9,  8,  9,  3,  3,  3,  3, 
         20, 20, 20, 20, 18, 17, 18, 17, 
         10, 11, 10, 11,  4,  4,  4,  4, 
          0,  0,  0,  0,  1,  1,  1,  1, 
         12, 13, 12, 13,  5,  5,  5,  5, 
          6,  6,  6,  6,  7,  7,  7,  7, 
         20, 14, 20, 14,  2,  2,  2,  2, 
         19, 15, 19, 15,  1, 16,  1, 16, 
         21}
}, 

{ /* 3: MITOCHONDRIAL FILAMENTOUS FUNGI */
        {"Mitochondrial Filamentous Fungi"}, 
        {"UGA=W"},
        { 8,  9,  8,  9,  3,  3,  3,  3, 
          0,  2,  0,  2, 17, 17, 18, 17,
         10, 11, 10, 11,  4,  4,  4,  4, 
          0,  0,  0,  0,  1,  1,  1,  1, 
         12, 13, 12, 13,  5,  5,  5,  5, 
          6,  6,  6,  6,  7,  7,  7,  7, 
         20, 14, 20, 14,  2,  2,  2,  2, 
         19, 15, 19, 15,  1, 16,  1, 16, 
         21}
}, 

{ /* 4: MITOCHONDRIAL CODE OF INSECT AND PLATYHELMINTHES  */
        {"Mitochondrial Insects and Platyhelminthes"}, 
        {"AUA=M  UGA=W  AGR=S"},
        { 8,  9,  8,  9,  3,  3,  3,  3, 
          2,  2,  2,  2, 18, 17, 18, 17, 
         10, 11, 10, 11,  4,  4,  4,  4, 
          0,  0,  0,  0,  1,  1,  1,  1, 
         12, 13, 12, 13,  5,  5,  5,  5, 
          6,  6,  6,  6,  7,  7,  7,  7, 
         20, 14, 20, 14,  2,  2,  2,  2, 
         19, 15, 19, 15,  1, 16,  1, 16, 
         21}
}, 

{ /* 5: CANDIDA CYLINDRACEA (see nature 341:164) */
        {"Candida cylindracea"}, 
        {"CUG=S  CUA=?"},
        { 8,  9,  8,  9,  3,  3,  3,  3, 
          0,  2,  0,  2, 17, 17, 18, 17,
         10, 11, 10, 11,  4,  4,  4,  4, 
          0,  0,  0,  0, 21,  1,  2,  1, 
         12, 13, 12, 13,  5,  5,  5,  5, 
          6,  6,  6,  6,  7,  7,  7,  7, 
         20, 14, 20, 14,  2,  2,  2,  2, 
         20, 15, 19, 15,  1, 16,  1, 16, 
         21}
}, 

{ /* 6: CILIATA */
        {"Ciliata"}, 
        {"UAR=Q"},
        { 8,  9,  8,  9,  3,  3,  3,  3, 
          0,  2,  0,  2, 17, 17, 18, 17,
         10, 11, 10, 11,  4,  4,  4,  4, 
          0,  0,  0,  0,  1,  1,  1,  1, 
         12, 13, 12, 13,  5,  5,  5,  5, 
          6,  6,  6,  6,  7,  7,  7,  7, 
         10, 14, 10, 14,  2,  2,  2,  2, 
         20, 15, 19, 15,  1, 16,  1, 16, 
         21}
}, 

{ /* 7: EUPLOTES */
        {"Euplotes"}, 
        {"UGA=C"},
        { 8,  9,  8,  9,  3,  3,  3,  3, 
          0,  2,  0,  2, 17, 17, 18, 17,
         10, 11, 10, 11,  4,  4,  4,  4, 
          0,  0,  0,  0,  1,  1,  1,  1, 
         12, 13, 12, 13,  5,  5,  5,  5, 
          6,  6,  6,  6,  7,  7,  7,  7, 
         20, 14, 20, 14,  2,  2,  2,  2, 
         15, 15, 19, 15,  1, 16,  1, 16, 
         21}
}, 

{ /* 8: MITOCHONDRIAL ECHINODERMS */
        {"Mitochondrial Echinoderms"}, 
        {"UGA=W  AGR=S  AAA=N"},
        { 9,  9,  8,  9,  3,  3,  3,  3, 
          2,  2,  2,  2, 17, 17, 18, 17, 
         10, 11, 10, 11,  4,  4,  4,  4, 
          0,  0,  0,  0,  1,  1,  1,  1, 
         12, 13, 12, 13,  5,  5,  5,  5, 
          6,  6,  6,  6,  7,  7,  7,  7, 
         20, 14, 20, 14,  2,  2,  2,  2, 
         19, 15, 19, 15,  1, 16,  1, 16, 
         21}
}

/* end of codes */ };
#endif
#endif
