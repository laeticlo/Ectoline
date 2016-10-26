/* -----------------------------------------------------------------------
 * $Id: LX.h 1780 2013-02-18 00:50:17Z viari $
 * -----------------------------------------------------------------------
 * @file: LX.h
 * @desc: Helix Utilities
 * 
 * @history:
 * @+ <Gloup> : Feb 02 : first version
 * @+ <Wanou> : Mar 02 : LXPat added
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * Helix utilities functs library<br>
 *
 * note: [AV] this is the master include file for all LX headers,
 *       please don't put anything else than #include here
 *
 * @docend:
 */

#ifndef _H_LX

#define _H_LX

#include "LX/LXMach.h"     /* machine specific flags       */
#include "LX/LXTypes.h"    /* standard types               */
#include "LX/LXMacros.h"   /* standard macros              */

#include "LX/LXSys.h"      /* system utilities             */

#include "LX/LXUtils.h"    /* misc. utilities              */

#include "LX/LXMath.h"     /* mathematical utilities       */

#include "LX/LXRand.h"     /* random numbers utilities     */

#include "LX/LXArray.h"    /* array utilities              */
#include "LX/LXStkp.h"     /* stack utilities              */
#include "LX/LXHeap.h"     /* heap utilities               */
#include "LX/LXList.h"     /* list utilities               */
#include "LX/LXDict.h"     /* dictionnary utilities        */

#include "LX/LXBStream.h"  /* byte stream IO               */

#include "LX/LXZlib.h"     /* zlib compression utilities   */

#include "LX/LXString.h"   /* string utilities             */
#include "LX/LXRegexp.h"   /* regular expression utilities */

#include "LX/LXXml.h"      /* XML utilities                */

#include "LX/LXPat.h"      /* Manber Algorithm & utilities */

#include "LX/LXFasta.h"    /* fasta format io              */
#include "LX/LXAbin.h"     /* abin format io               */

#include "LX/LXGenetic.h"  /* genetic code utilities       */
#include "LX/LXBio.h"      /* bio utilities                */

#endif



