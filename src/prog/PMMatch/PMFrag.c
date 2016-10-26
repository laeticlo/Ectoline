/* -----------------------------------------------------------------------
 * $Id: PMFrag.c 1366 2011-08-22 16:40:49Z viari $
 * -----------------------------------------------------------------------
 * @file: PMFrag.c
 * @desc: Fragmentation model
 *
 * @history:
 * @+ <JeT> : Jan 07 : first version
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX.h"
#include "PM.h"

#include "PMFrag.h"

#define EPSILON 1e-6

/* -------------------------------------------- */
/* external table of fragment symbols           */
/* -------------------------------------------- */
char gFragmentTypeSymbol[NB_FRAG_TYPES][2] = 
{
	"f", "a", "b", "c", "x", "y", "z", "i"
};

//
// [AV] FIXME everywhere : use comment headers consistently
//

/*
 * compute the sum of amino acids residues of sequence
 * return a neutral mass
 */

static float sGetResidueMass(char *sequence, PMAminoAlphabet *alpha)
{
  float mass = 0.;
  
  for ( ; *sequence ; sequence++) {
  
    PMAminoAcid *aa;
    
    if (! (aa = PMGetAminoAcid(alpha, *sequence))) {
      fprintf(stderr, "# Invalid AminoAcid : '%c' (%d)\n", *sequence, *sequence);
      return -1.;
    }

    mass += aa->mass;
  }
  
  return mass;
}

/*
 * Generates the Y fragment ion of the sequence
 * return the H+ ion m/z 
 */

static PMFragmentPtr sGenerateYFragment(char *sequence, PMAminoAlphabet *alpha )
{
  float mass = sGetResidueMass(sequence, alpha);

  if (mass < 0) {
    fprintf(stderr, "# Error generating Y fragment for sequence %s\n", sequence);
    return NULL;
  }

  PMFragmentPtr frag = PMNewFragment();
  
  frag->type  = Y_Fragment;
  frag->mass  = mass + PM_MASS_H2O + PM_MASS_H;
  frag->index = strlen(sequence);
  
  return frag;
}

/*
 * Generates the B fragment ion of the sequence
 * return the H+ ion m/z 
 */

static PMFragmentPtr sGenerateBFragment( char *sequence, PMAminoAlphabet *alpha )
{
  float mass = sGetResidueMass(sequence, alpha);

  if (mass < 0) {
    fprintf(stderr, "# Error generating B fragment for sequence %s\n", sequence);
    return NULL;
  }
  
  PMFragmentPtr frag = PMNewFragment();

  frag->type  = B_Fragment;
  frag->mass  = mass + PM_MASS_H;
  frag->index = strlen(sequence);
  
  return frag;
}

/* ============================================ */
/* API										    */
/* ============================================ */

/* -------------------------------------------- */
/* create a new fragment                        */
/* -------------------------------------------- */

PMFragmentPtr PMNewFragment()
{

  PMFragmentPtr frag;	

  if (! (frag = NEW(PMFragment))) {
    MEMORY_ERROR();
    return NULL;
  }

  frag->mass = 0.;
  frag->peak = NULL;
  frag->type = CompleteFragment;
  
  return frag;
}

/* -------------------------------------------- */
/* free fragment                        		*/
/* -------------------------------------------- */

PMFragmentPtr PMFreeFragment(PMFragmentPtr frag)
{
  if (frag)
    free(frag);
  
  return NULL;
}

/* -------------------------------------------- */
/*  compar two fragments on masses              */
/* -------------------------------------------- */

int PMCompareFragmentMasses(const void *fragment1, const void *fragment2 )
{
  float mass1 = ( *( (PMFragmentPtr const * ) (fragment1)))->mass;
  float mass2 = ( *( (PMFragmentPtr const * ) (fragment2)))->mass;

  return ((mass1 < mass2) ? -1 : (mass1 > mass2) ? 1 : 0);
}

/* -------------------------------------------- */
/*  print fragment debug information            */
/* -------------------------------------------- */

void PMDebugFragment(FILE *streamou, PMFragment *frag)
{
  fprintf(streamou, "// fragment\n");
  fprintf(streamou, "// mass          : %.3f\n", frag->mass );
  // [AV] FIXME : %p n'est pas standard : que veux tu faire exactement ?
  fprintf(streamou, "// peak          : %p", frag->peak );
  if (frag->peak) fprintf(streamou, "( %.3f/%.3f : #%d)",
  	                      frag->peak->mass, frag->peak->intensity, frag->peak->rank);
  fprintf(streamou, "\n" );
  fprintf(streamou, "// type          : %s%d\n", gFragmentTypeSymbol[frag->type], frag->index );
  fprintf(streamou, "//\n");
}

/*
 * compute the peptide mass by summing residues
 * and adding H20 (H- aa -HO)
 * return the neutral mass of the complete peptide
 */

float PMGetPeptideMass(char *sequence, PMAminoAlphabet *alpha)
{
  float mass = sGetResidueMass(sequence, alpha);
  
  if (mass < 0)
    return mass;
    
  return (mass + PM_MASS_H2O);
}

 /*
  * generate a collection of fragments
  * from a sequence. use a fragmentation model
  */
  
LXStackpPtr PMGenerateFragments(char *sequence, PMAminoAlphabet *alpha)
{
  char *s;

  LXStackpPtr fragments; 
  
  if (! (fragments = LXNewStackp(LX_STKP_MIN_SIZE))) {
    MEMORY_ERROR();
    return NULL;
  }

  for (s = sequence + 1 ; *s ; s++) {
    PMFragmentPtr frag = sGenerateYFragment(s, alpha);
    if (frag) LXPushpIn(fragments, frag);
  }

  for (s = LXReverseStr(sequence) + 1 ; *s ; s++) {
    PMFragmentPtr frag = sGenerateBFragment(s, alpha);
    if (frag) LXPushpIn(fragments, frag);
  }

  (void) LXReverseStr(sequence); // put it back to correct order

  return fragments;
}


  /*
   * Assign spectrum peaks to the fragments
   */
   
int PMAssignPeaks(PMSpectrum *spectrum, LXStackpPtr fragments, Parameter *param)
{
  PMFragmentPtr frag;

  LXCurspToBottom(fragments);
  
  while (LXReadpUp(fragments, (Ptr *) (&frag))) {

    if (frag->mass < EPSILON)
        continue;

    // [AV] FIXME :
    //
    //  - peak searching is too brutal : should be O(log(n))
    //  - peak selection is too simple : it takes the first match
    
    int iPeak;
    for (iPeak = 0 ; iPeak < spectrum->nbPeaks ; iPeak++) {
    
      float peakMass = spectrum->peak[iPeak].mass;
      float delta = ABS(peakMass - frag->mass);
      if (param->massTolerance >= 0)
        delta /= frag->mass;

      if (delta < param->massTolerance) {
        frag->peak = spectrum->peak + iPeak;
        break;
      }
      else if (peakMass > frag->mass) // peaks are sorted by increasing masses
        break;                        // so we don't need to go further
      
    }
  }
  
  return LX_NO_ERROR;
}
