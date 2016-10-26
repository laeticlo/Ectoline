/* -----------------------------------------------------------------------
 * $Id: PMEnzyme.c 1810 2013-02-25 10:04:43Z viari $
 * -----------------------------------------------------------------------
 * @file: PMEnzyme.c
 * @desc: PepMap enzyme library
 * 
 * @history:
 * @+ <Wanou> : Jan 02 : first version 
 * @+ <Gloup> : Jul 06 : code cleanup
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * PM enzyme library<br>
 *
 * @docend:
 */

#include <string.h>

#include "PM/PMSys.h"
#include "PM/PMEnzyme.h"

#define LOCAL_ERROR(msg, enz) sIOError(__FILE__, __LINE__, msg, enz)

static char *sCurrentLine = NULL;

/* -------------------------------------------- */
/* read error                                   */
/* -------------------------------------------- */

static int sIOError(const char *fileName, int lineno, const char *msg, const char *enz)
{
  char buffer[BUFSIZ];
  
  if (sCurrentLine) {
    strncpy(buffer, sCurrentLine, strlen(sCurrentLine) - 1);
    (void) fprintf(stderr, "// *Error* at or near line : \"%s\"\n", buffer);
  }
    
  (void) sprintf(buffer, "IO error : %s for enzyme \"%s\"", msg, (enz ? enz : "<unknown>"));

  return LXIOError(fileName, lineno, buffer, LX_IO_ERROR);
}

/* -------------------------------------------- */
/* get next input line                          */
/* -------------------------------------------- */

static char *sNextLine(FILE *streamin)
{
  static char buffer[BUFSIZ];
  
  sCurrentLine = buffer;

  return LXGetLine(buffer, sizeof(buffer), streamin);
}


/* -------------------------------------------- */
/* IO funcs                                     */
/* -------------------------------------------- */

/* ---------------------------------------------------- */
/* read enzyme from file                                */
/* ---------------------------------------------------- */

int PMReadEnzyme(FILE *streamin, PMEnzyme *enzyme)
{
  int i;
  char *buffer;

  /* ------------------ */
  /* reset enzyme       */

  (void) memset(enzyme, 0, sizeof(PMEnzyme));

  /* ------------------ */
  /* read name or EOF   */

  if (! (buffer = sNextLine(streamin)))
    return LX_EOF;
    
  if (sscanf(buffer, "%s%d%f", enzyme->name, &(enzyme->nbCuts), &(enzyme->probOver)) != 3)
    return LOCAL_ERROR("syntax error", NULL);
    
  /* ------------------ */
  /* read cut sites     */

  for (i = 0 ; i < enzyme->nbCuts ; i++) {

    PMCutSite *site = enzyme->cutSite + i;
    
    if (! (buffer = sNextLine(streamin)))
      return LOCAL_ERROR("cannot read cut sites", enzyme->name);

    if (sscanf(buffer,"%f%d%s%s", &(site->probCut), &(site->NCutOffset), 
                                  site->NCut, site->CCut) != 4)
      return LOCAL_ERROR("bad number cut site", enzyme->name);

    site->probMis = 1.0 - site->probCut;
    site->NCutRegExp = LXRegComp(site->NCut);
    site->CCutRegExp = LXRegComp(site->CCut);

    if (! (site->NCutRegExp && site->CCutRegExp))
      return LOCAL_ERROR("invalid cut site regexp", enzyme->name);
  }

  /* ------------------ */
  /* end                */
  
  return LX_NO_ERROR;
}

/* -------------------------------------------- */
/* load enzyme from file                        */
/* -------------------------------------------- */

int PMLoadEnzyme(char *filename, PMEnzyme *enzyme)
{
  FILE *enzFile;
 
  if (! (enzFile = LXOpenFile(filename, "r")))
    return IO_ERROR();

  int res = PMReadEnzyme(enzFile, enzyme);
  
  (void) fclose(enzFile);

  return res;
}

/* -------------------------------------------- */
/* debug funcs                                  */
/* -------------------------------------------- */


/* ---------------------------------------------------- */
/* print enzyme debug information                       */
/* ---------------------------------------------------- */

void PMDebugEnzyme(FILE *streamou, PMEnzyme *enzyme)
{
    int i;

    fprintf(streamou, "// PMEnzyme\n");
    fprintf(streamou, "// name      : %s\n", enzyme->name);
    fprintf(streamou, "// nbCuts    : %d\n", enzyme->nbCuts);
    fprintf(streamou, "// probOver  : %f\n", enzyme->probOver);

    for (i = 0 ; i < enzyme->nbCuts ; i++) {
    
      PMCutSite *cut = enzyme->cutSite + i;

      fprintf(streamou, "// cut %2d   : Offset=%d NCut=\"%s\" CCut=\"%s\" probCut=%f probMis=%f\n",
                        i, cut->NCutOffset, cut->NCut, cut->CCut, cut->probCut, cut->probMis);
    }
    fprintf(streamou, "//\n");
}
