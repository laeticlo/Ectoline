/* -----------------------------------------------------------------------
 * $Id: PM.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PM.h
 * @desc: PM (PepMap Module) library
 *
 * @history:
 * @+ <Wanou> : Dec 01 : first version
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PM (PepMap Module) library<br>
 * It defines aa, tag, hit & match types & associated i/o functions 
 *
 * @docend:
 */

#ifndef _H_PM
#define _H_PM

#ifndef _H_LX
#include "LX.h"
#endif

#include "PM/PMSys.h" 		/* PM utilities			*/

#include "PM/PMIsotope.h" 	/* isotopic masses      */

#include "PM/PMAa.h"        /* amino acids          */
#include "PM/PMEnzyme.h"    /* enzymes              */

#include "PM/PMSpectrum.h"  /* spectrum             */

#include "PM/PMTag.h"       /* tags                 */
#include "PM/PMTagStack.h"  /* stack of tags        */

#include "PM/PMSeqId.h"     /* sequence identifiers */

#include "PM/PMHit.h"       /* hits                 */
#include "PM/PMHitStack.h"  /* stack of hits        */

#include "PM/PMUtil.h"      /* utilities            */

#endif
