/* -----------------------------------------------------------------------
 * $Id: PMHit.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMHit.h
 * @desc: hit (PepMap Module) library
 *
 * @history:
 * @+ <Wanou> : Dec 01 : first version
 * @+ <Gloup> : Feb 06 : modification of Hit structure
 * @+ <Gloup> : Jul 06 : added sequence header in hit
 * @+ <Gloup> : Aug 06 : code cleanup
 * @+ <Gloup> : Jan 10 : 64 bits polish
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * hit (PepMap Module) library<br>
 * It defines hit type & associated i/o functions 
 *
 * @docend:
 */

#ifndef _H_PMHit
#define _H_PMHit

#include "LX/LXSys.h"

#include "PMSys.h"
#include "PMTag.h"

/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/*
 * @doc: hit complete/partial type
 * - InvalidlHit    : invalid hit (partial both sides)
 * - NDefinedHit    : partial hit N-Terminal defined
 * - CDefinedHit    : partial hit C-Terminal defined
 * - FullHit        : complete hit
 */

typedef enum {
  InvalidHit = 0,
  NDefinedHit,
  CDefinedHit,
  FullHit
} PMHitType;

/*
 * @doc: Hit features
 * - type       : PMHitType - InvalidlHit, NDefinedHit, CDefinedHit, FullHit;
 * - posMn      : long  - position of the first aa of the tag
 * - posMc      : long  - position of the last aa of the tag
 * - posSn      : long  - position of the first lexeme (tag sequence)
 *                        of the tag
 * - posSc      : long  - position of the last lexeme (tag sequence)
 *                        of the tag
 * - nbMis      : int   - number of miscleavage (0, ... , n)
 * - nbOver     : int   - number of overcleavage (0, 1, 2)
 * - nbMod      : int   - number of modifications (0, ... , n)
 *
 * - score      : float - hit score 
 * - pepSeq     : char *  - hit sequence peptide (as found on chromosome)
 * - pepMatch   : char *  - hit matched peptide (i.e with possible modifications)
 *
 * - tag        : PMTag * - [optional] parent tag pointer
 */

typedef struct {                    /* -- Hit Structure --                  */

        PMHitType  type;            /* hit type                             */

        long       posMn,           /* position of the first aa of the tag  */
                   posMc,           /* position of the last aa of the tag   */
                   posSn,           /* position of the first lexeme         */
                   posSc;           /* position of the last lexeme          */

        int        nbMis,           /* number of miscleavage                */
                   nbOver,          /* number of overcleavage               */
                   nbMod;           /* number of modifications              */

        float      score;           /* hit score                            */

        char       *pepSeq;         /* hit sequence peptide                 */

        char       *pepMatch;       /* hit matched peptide                  */

                                    /* ------------------------------------ */
                                    /* the following fields are  optional.  */
                                    /* They are not handled by the library  */
                                    /* and are left to your responsability  */
                                    /* ------------------------------------ */
        PMTag      *tag;            /* parent tag pointer                   */
        
} PMHit, *PMHitPtr;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * Allocate a new PMHit
 * @return PMHit*
 */

PMHit *PMNewHit ( void );

/*
 * @doc: 
 * Free a previously allocated hit structure
 * @return NULL
 */

PMHit *PMFreeHit ( PMHit *hit );

/*
 * @doc:
 * Read a hit from buffer 
 * @param buffer char* to read in
 * @param hit PMHit* hit to read
 * @return error code depending on read issue
 * LX_NO_ERROR, LX_IO_ERROR
 */

int  PMParseHit ( char *buffer, PMHit *hit );

/*
 * @doc:
 * Read next hit in file (serial reading)
 * @param streamin FILE* to read in
 * @param hit PMHitPtr hit to read
 * @return error code depending on read issue
 * LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */

int  PMReadHit  ( FILE *streamin, PMHit *hit );

/*
 * @doc:
 * Write hit into file
 * @param streamou FILE* to write into
 * @param hit PMhitPtr hit to write
 * @return error code depending on write issue
 * LX_NO_ERROR, LX_IO_ERROR
 */

int  PMWriteHit  ( FILE *streamou, PMHit *hit );

/*
 * @doc:
 * Write end of hits mark into file
 * @param streamou FILE* to write into
 * @return error code depending on write issue
 * LX_NO_ERROR, LX_IO_ERROR
 */

int  PMEndWriteHit  ( FILE *streamou );

/*
 * @doc: 
 * write binary data structure hit in file streamou
 * @param streamou FILE* binary opened file to write into
 * @param hit PMHit* ptr on hit structure to be dumped 
 * @return number of bytes written (> 0), <= 0 on io error
 */
 
int PMBinWriteHit (FILE *streamou, PMHit *hit);

/*
 * @doc: 
 * read binary data structure hit from file streamin
 * @param streamin FILE* binary opened file to read into
 * @param hit PMHit* ptr on hit structure to be loaded 
 * @return number of bytes read (> 0), <= 0 on io error
 */
int PMBinReadHit (FILE *streamin, PMHit *hit);

/*
 * @doc:
 * [debug] print PMHit debug information
 * @param streamou FILE* to write into
 * @param hit PMHit* hit to write
 */

void PMDebugHit ( FILE *streamou, PMHit *hit );

#endif

