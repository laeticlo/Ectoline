/* -----------------------------------------------------------------------
 * $Id: PMAa.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMAa.h
 * @desc: aminoacid library
 *
 * @history:
 * @+ <Wanou> : Dec 01 : first version
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * amino acid alphabet (PepMap Module) library<br>
 * It defines PMAAAlphabet & associated i/o functions 
 *
 *
 * @docend:
 */

#ifndef _H_PMAa

#define _H_PMAa

#include "PMSys.h"

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @doc: maximum length of Amino-Acid Alphabet (including modified aa)
 *  - do not change - since the alphabet is indexed by ascii code
 */

#define PM_MAX_AA          128  

/*
 * @doc: maximum length for Amino-Acid name
 */
 
#define PM_AA_MAX_NAME_LEN  32

/*
 * @doc: maximum number of modifications per aa
 */

#define PM_MAX_MOD_PER_AA  8

/*
 * @doc: Code for Amino-Acid type
 * - PM_AA_ORIGIN        : Original (no modification)
 * - PM_AA_MODIFIED      : Modified
 * - PM_AA_PSEUDO        : Pseudo AA
 */

#define PM_AA_ORIGIN          0
#define PM_AA_MODIFIED        1
#define PM_AA_PSEUDO          2

/*
 * @doc: Code for Amino-Acid modification location
 * - PM_AA_MOD_LOC_ANY   : Any location
 * - PM_AA_MOD_LOC_NTERM : N terminal modification
 * - PM_AA_MOD_LOC_CTERM : C terminal modification
 */

#define PM_AA_MOD_LOC_ANY     0
#define PM_AA_MOD_LOC_NTERM   1
#define PM_AA_MOD_LOC_CTERM   2

/*
 * @doc: Start, Stop, Error Codons AminoAcids
 */

#define PM_START_CODON_AA   'M'

#define PM_STOP_CODON_AA    LX_GENETIC_STOP_AA

#define PM_ERROR_CODON_AA	LX_GENETIC_BAD_AA

/*
 * @doc: 'Infinite' mass
 */

#define PM_BIG_MASS 1e6

/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/*
 * @doc: Amino Acid symbol representation
 *       note: actual size is char but we use int for ANSI compatibility
 */

typedef int PMSymbol;

/*
 * @doc: Amino Acid structure
 * - type          : int - PM_AA_ORIGIN | PM_AA_MODIFIED | PM_AA_PSEUDO 
 * - oneLetterCode : PMSymbol - amino acid 1 letter code
 * - mass          : float - mass of aa
 * - name          : char[PM_AA_MAX_NAME_LEN] - aa full name
 * - nbMod         : int - number of modifications 
 * - modIndx       : int[PM_MAX_MOD_PER_AA] - index of modified versions of this aa
 *
 * ---------- for PM_AA_MODIFIED aa
 *
 * - origin        : PMSymbol - original (unmodified) aa code
 * - modLocation   : int - modification location
 * - modProba      : float - probability of modification
 */

typedef struct {
    int         type;
    PMSymbol    oneLetterCode;
    float       mass;
    int         nbMod;
    int         modIndx[PM_MAX_MOD_PER_AA];
    PMSymbol    origin;
    int         modLocation;
    float       modProba;
    char        name[PM_AA_MAX_NAME_LEN+2];
} PMAminoAcid;

/*
 * @doc: Amino Acid Alphabet
 * - valid : Bool[PM_MAX_AA] - validity of ith element
 * - table : PMAminoAcid[PM_MAX_AA] - table of amino acids
 */

typedef struct {
    Bool            valid[PM_MAX_AA];
    PMAminoAcid     table[PM_MAX_AA];
} PMAminoAlphabet;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc:
 * Read a PMAminoAlphabet from stream
 * @param streamin FILE* to read in   
 * @param alpha PMAminoAlphabet* to read
 * @return error code (LX_NO_ERROR if no error)
 */

int PMReadAminoAlphabet(FILE *streamin, PMAminoAlphabet *alpha);

/*
 * @doc:
 * Read a PMAminoAlphabet from file named filename
 * @param filename name of file to read in   
 * @param alpha PMAminoAlphabet* to read
 * @return error code (LX_NO_ERROR if no error)
 */

int PMLoadAminoAlphabet(char *filename, PMAminoAlphabet *alpha);

/*
 * @doc:
 * returns PMAminoAcid* corresponding to PMSymbol code
 * @param alpha PMAminoAlphabet* alphabet 
 * @param code PMSymbol one letter code of aminoacid to get
 * @return PMAminoAcid* corresponding to PMSymbol code.
 *         NULL if the given code is not defined in the alphabet.
 */

PMAminoAcid *PMGetAminoAcid(PMAminoAlphabet *alpha, PMSymbol code);

/*
 * @doc:
 * get complete alphabet (including modified aa)
 * @param alpha PMAminoAlphabet* alphabet 
 * @return a static string (make a copy of it) representing the alphabet.
 */

char *PMGetFullAlphabet(PMAminoAlphabet *alpha);

/*
 * @doc:
 * get alphabet of normal aminoacids
 * @param alpha PMAminoAlphabet* alphabet 
 * @return a static string (make a copy of it) representing the alphabet.
 */

char *PMGetNormalAlphabet(PMAminoAlphabet *alpha);

/*
 * @doc:
 * [debug] print PMAminoAcid debug information
 * @param streamou FILE* to write into
 * @param aa PMAminoAcid* pointer to aminoacid 
 */

void PMDebugAminoAcid(FILE *streamou, PMAminoAcid *aa);

/*
 * @doc:
 * [debug] print PMAminoAcid debug information
 * @param streamou FILE* to write into
 * @param alpha PMAminoAlphabet* pointer to alphabet 
 */

void PMDebugAminoAlphabet(FILE *streamou, PMAminoAlphabet *alpha);

#endif
