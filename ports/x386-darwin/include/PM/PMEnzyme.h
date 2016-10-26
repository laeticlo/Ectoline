/* -----------------------------------------------------------------------
 * $Id: PMEnzyme.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMEnzyme.h
 * @desc: PepMap enzyme structure and associated FIOs
 * 
 * @history:
 * @+ <Wanou> : Jan 02 : first version 
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * PM enzyme library<br>
 * structure defintion and functions
 *
 * $Log: PMEnzyme.h,v $
 * Revision 1.1  2003/10/23 15:05:11  reguer
 *
 * First commit on Taggor repository
 * ER INRIA-RA/CEA 2003
 *
 * Revision 1.1.1.1  2002/03/22 13:18:10  reguer
 * Release 1.0
 *
 *
 * @docend:
 */

#ifndef _H_PMEnzyme
#define _H_PMEnzyme

#include "LX/LXSys.h"
#include "LX/LXRegexp.h"

#include "PMSys.h"

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @doc: maximum regexp length (in chars)
 */
#define PM_ENZYME_MAX_REG_LEN 64

/*
 * @doc: enzyme maximum name length
 */
#define PM_ENZYME_MAX_NAME_LEN 64

/*
 * @doc: maximum number of "cut sites" per enzyme
 */
#define PM_ENZYME_MAX_CUT_SITES  8

/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/* @doc: cut site structure
 * - probCut       : float cut probability [0,1]
 * - probMis       : float miscleavage probability = 1 - probCut
 * - NCutOffset    : offset to nCut (usually 1)
 * - nCut          : char[PM_ENZYME_MAX_REG_LEN] regexp of the "N-cut"
 * - cCut          : char[PM_ENZYME_MAX_REG_LEN] regexp of the "C-cut"
 * - nCutRegExp    : LXRegexp compiled "N-cut" regexp
 * - cCutRegExp    : LRregexp compiled "C-cut" regexp
 */
typedef struct {
  float     probCut,
            probMis;
  int       NCutOffset;
  char      NCut[PM_ENZYME_MAX_REG_LEN + 2],
            CCut[PM_ENZYME_MAX_REG_LEN + 2];
  LXRegExp  *NCutRegExp,
            *CCutRegExp;
} PMCutSite, *PMCutSitePtr;

/*
 * @doc: Enzyme data structure
 * - name          : char[PM_ENZYME_MAX_NAME_LEN] - enzyme name
 * - nbCuts        : int - number of cut sites
 * - probOver      : float - Probability of overcleavage
 * - cutSite       : PMCutSite[PM_ENZYME_MAX_CUT_SITES]
 */

typedef struct {
  int           nbCuts;
  float         probOver;
  PMCutSite     cutSite[PM_ENZYME_MAX_CUT_SITES];
  char          name[PM_ENZYME_MAX_NAME_LEN + 2];
} PMEnzyme, *PMEnzymePtr;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc:
 * Read Enzyme data from stream
 * @param streamin FILE* to read in
 * @param enzyme   PMEnzyme* to read
 * @return error code depending upon read issue :
 * LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */

int PMReadEnzyme(FILE *streamin, PMEnzyme *enzyme); 

/*
 * @doc:
 * Read Enzyme data from file name filename
 * @param filename char * filename to read from
 * @param enzyme   PMEnzyme* to read
 * @return error code depending upon read issue :
 * LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */

int PMLoadEnzyme(char *filename, PMEnzyme *enzyme); 

/*
 * @doc:
 * [debug] print PMEnzyme debug information
 * @param streamou FILE* to write into
 * @param enzyme   PMEnzyme* to print
 */

void PMDebugEnzyme(FILE *streamou, PMEnzyme *enzyme);

#endif
