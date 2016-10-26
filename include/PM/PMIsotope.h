/* -----------------------------------------------------------------------
 * $Id: PMIsotope.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMIsotope.h
 * @desc: Monoisotopic masses of usual elements
 *
 * @history:
 * [AV] a revoir
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PM Masses of usual elements
 *
 * @docend:
 */

#ifndef _H_PMIsotope
#define _H_PMIsotope

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @doc: Mass of Hydrogen 1 (in amu)
 */

#define PM_MASS_H       1.0078250

/*
 * @doc: Mass of Carbon 12 (in amu)
 */

#define PM_MASS_C       12.0000000

/*
 * @doc: Mass of Oxygen 16 (in amu)
 */

#define PM_MASS_O       15.9949146 

/*
 * @doc: Mass of Nitrogen 14 (in amu)
 */

#define PM_MASS_N       14.0030740 

/* ********************************
 * shorthands
 */

/*
 * @doc: Mass of H2 (in amu)
 */

#define PM_MASS_H2 (PM_MASS_H + PM_MASS_H)
 
/*
 * @doc: Mass of HO (in amu)
 */

#define PM_MASS_HO (PM_MASS_H + PM_MASS_O)

/*
 * @doc: Mass of H2O (in amu)
 */

#define PM_MASS_H2O (PM_MASS_H + PM_MASS_HO)

/*
 * @doc: Mass of H3O (in amu)
 */

#define PM_MASS_H3O (PM_MASS_H + PM_MASS_H2O)

/*
 * @doc: Mass of HN (in amu)
 */

#define PM_MASS_HN (PM_MASS_H + PM_MASS_N)

/*
 * @doc: Mass of H2N (in amu)
 */

#define PM_MASS_H2N (PM_MASS_H + PM_MASS_HN)


#endif
