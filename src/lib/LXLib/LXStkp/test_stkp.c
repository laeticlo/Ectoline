/* test libstkp */

#include <stdio.h>
#include <stdlib.h>

#include "LX/LXStkp.h"

#define VMAX 100
#define NMAX 400000
#define STEP 50000
#define VSTEP 10

/* ----------------------------------------------- */

main(argn, argv)
	int	 argn;
	char *argv[];
{
	int			 i, j;
	LXStackpPtr	 stk;
	LXStackpHdle sth;

	/* --------- stack test ---------- */

	stk = LXNewStackp(LX_STKP_MIN_SIZE);

	(void) LXUserCpuTime(LX_TIME_RESET);
	(void) LXSysCpuTime(LX_TIME_RESET);

	for (i = 0 ; i < NMAX ; i++) {

	   LXPushpIn(stk, NULL);

	   if ((i%STEP) == (STEP-1))
			printf("STACK: %d %s\n", i+1,
			   LXStrCpuTime(LX_TIME_NO_RESET));
	}

	LXFreeStackp(stk, NULL);

	/* --------- stack vector test	---------- */

	(void) LXUserCpuTime(LX_TIME_RESET);
	(void) LXSysCpuTime(LX_TIME_RESET);

	sth = LXNewStackpVector(VMAX, LX_STKP_MIN_SIZE);

	for (j = 0 ; j < VMAX ; j++) {
	
		for (i = 0 ; i < NMAX/VMAX ; i++)
		    LXPushpIn(sth[j], NULL);

		if ((j%VSTEP) == (VSTEP-1))
		printf("VSTACK: %d %s\n", j+1,
			LXStrCpuTime(LX_TIME_NO_RESET));
	}

	LXFreeStackpVector(sth, VMAX, NULL);
	
	exit(0);
}
