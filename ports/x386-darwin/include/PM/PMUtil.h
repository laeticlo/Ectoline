/* -----------------------------------------------------------------------
 * $Id: PMUtil.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMUtil.h
 * @desc: utilities (PepMap Module) library
 *
 * @history:
 * @+ <Gloup> : Feb 07 : added
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * @docend:
 */

#ifndef _H_PMUtil
#define _H_PMUtil

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @doc: mass offset to compute deltaM from Mass
 * deltaM = (mass + PM_DMM_OFFSET) * dm/m
 *
 * this avoids deltaM -> 0 when mass -> 0
 *
 * [AV]: PepMatch 2.0 behavior was PM_DMM_OFFSET = 0
 */

#define PM_DMM_OFFSET  100

#endif

