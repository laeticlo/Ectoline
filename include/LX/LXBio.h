/* -----------------------------------------------------------------------
 * $Id: LXBio.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: LXBio.h
 * @desc: genetic library/ include file
 * 
 * @history:
 * @+ <Gloup> : Jan 96 : first version for PWG 
 * @+ <Wanou> : Jan 02 : adapted to LX_Ware 
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * Genetic library / include file<br>
 *
 * @docend:
 */

#ifndef _H_LXBio

#define _H_LXBio

#ifndef _H_LXString
#include "LXString.h"
#endif

#ifndef _H_LXGenetic
#include "LXGenetic.h"
#endif

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @doc: DNA alphabet (IUPAC)
 */
#define LX_BIO_DNA_ALPHA   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

/*
 * @doc: complementary DNA alphabet (IUPAC)
 */
#define LX_BIO_CDNA_ALPHA  "TVGHEFCDIJMLKNOPQYSAABWXRZtvghefcdijmlknopqysaabwxrz"

/*
 * @doc: codon length (in bases)
 */
#define LX_BIO_CODON_LENGTH   3

/*
 * @doc: sequence type
 */
 
#define LX_BIO_SEQ_UNKNOWN    0
#define LX_BIO_SEQ_DNA        1
#define LX_BIO_SEQ_PROTEIN    2

/* ---------------------------------------------------- */
/* Prototypes of library functions                      */
/* ---------------------------------------------------- */

/*
 * @doc:
 * get DNA/RNA-Iupac base complement
 * @param nucAc char nucleic acid to find complement
 * @return nucleic acid complement
 */                      
char LXBioBaseComplement (char nucAc);

/*
 * @doc:
 * get DNA/RNA-Iupac sequence complement
 * @param nucAcSeq char* nucleic acid sequence
 * @return nucleic acid sequence complement
 * warning: just complement, do not reverse (see LXBioSeqReverseComplement)
 */                      
char *LXBioSeqComplement (char *nucAcSeq);

/*
 * @doc:
 * get DNA/RNA-Iupac sequence reverse complement
 * @param nucAcSeq char* nucleic acid sequence
 * @return nucleic acid sequence reverse complement
 */                      
char *LXBioSeqReverseComplement(char *nucAcSeq);

/*
 * @doc:
 * translate given codon according to chosen genetic code (please refer to LXgenetic.h)
 * @param codon *char codon to translate
 * @param codid int genetic code identifier
 * @return amino acid char
 */                      
char LXBioCodonTranslate (char *codon, int codid);

/*
 * @doc:
 * translate given nucleic acid sequence according to chosen genetic code (please refer to LXgenetic.h)
 * @param nucAcSeq char* sequence to translate
 * @param codid int genetic code identifier
 * @return amino acid sequence 
 */
char *LXBioSeqTranslate (char *nucAcSeq, int codid);

/*
 * @doc:
 * try to guess the type (i.e. nucleic or proteic) of a biosequence.
 * This is a heuristic based on symbol composition. It basically
 * check if the 'T'+'C' ratio is greater than a given fraction.
 * @param seq char* sequence to check
 * @return type LX_BIO_SEQ_DNA or LX_BIO_SEQ_PROTEIN
 */
int LXBioSeqType(char *seq);

#endif
