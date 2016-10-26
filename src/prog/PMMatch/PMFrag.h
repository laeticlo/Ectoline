/* -----------------------------------------------------------------------
 * $Id: PMFrag.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMFrag.h
 * @desc: PMFrag library
 *
 * @history:
 * @ <JeT> : Jan 07 : creation
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PMFrag library <br>
 *
 * @docend:
 */

#ifndef _H_PMFrag
#define _H_PMFrag

#ifndef _H_LX
#include "LX.h"
#endif

#ifndef _H_PM
#include "PM.h"
#endif

#include "PMMatch.h"

/* ----------------------------------------------- */
/* constants                                       */
/* ----------------------------------------------- */

/* 
 * @doc: Default parameter values
 */

#define DFT_TOLERANCE   50

/* 
 * @doc: # of fragment types
 */

#define NB_FRAG_TYPES 8


/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/*
 * @doc: Types of fragments
 */

typedef enum {
    CompleteFragment = 0,
    A_Fragment,
    B_Fragment,
    C_Fragment,
    X_Fragment,
    Y_Fragment,
    Z_Fragment,
    ImmoniumFragment
} FragmentType;

/*
 * @doc: Fragment structure
 * a fragment is a subpart of a peptide
 * [AV] FIXME : please document structure here, not only below
 */
typedef struct {                        /* fragment structure                   */
    float               mass;           /* mass                                 */
    PMPeak              *peak;          /* associated peak                      */
    FragmentType        type;           /* fragment type  : A-C, X-Z            */
    int                 index;          /* fragment index : e.g 2 in Y2         */
} PMFragment, *PMFragmentPtr;

/* ----------------------------------------------- */
/* globals                                         */
/* ----------------------------------------------- */

extern char gFragmentTypeSymbol[NB_FRAG_TYPES][2];

/* ----------------------------------------------- */
/* prototypes                                      */
/* ----------------------------------------------- */

// [AV] FIXME : please document the functions with @doc, @param, @return
//
// use comments consistently
//
// declare function in the same order as in file.c
//

/*
 * doc
 */
PMFragmentPtr PMNewFragment();

/*
 * doc
 */
PMFragmentPtr PMFreeFragment(PMFragmentPtr fragment);


/*
 * doc
 */

int PMCompareFragmentMasses(const void *fragment1, const void *fragment2);

/* -------------------------------------------- */
/*  print fragment debug information            */
/* -------------------------------------------- */

void PMDebugFragment(FILE *streamou, PMFragment *fragment);


 /*
  * compute the sequence mass by suming residues
  */
   
float PMGetPeptideMass( char *sequence, PMAminoAlphabet *alpha);

 /*
  * generate a collections of fragment
  * from a sequence. use a fragmentation model
  */

LXStackpPtr PMGenerateFragments(char *sequence, PMAminoAlphabet *alpha);

  /*
   * Assign spectrum peaks to the fragments
   * assume that the spectrum and the fragments 
   * are sorted on masses
   */
   
int PMAssignPeaks(PMSpectrum *spectrum, LXStackpPtr fragments, Parameter *param);

#endif
