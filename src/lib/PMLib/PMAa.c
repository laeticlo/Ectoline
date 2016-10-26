/* -----------------------------------------------------------------------
 * $Id: PMAa.c 1810 2013-02-25 10:04:43Z viari $
 * -----------------------------------------------------------------------
 * @file: PMaa.c
 * @desc: PM Amino Acid library
 *
 * @history:
 * @+ <Wanou> : Dec 01 : first version
 * @+ <Gloup> : Jul 06 : code cleanup
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * PM Amino Acid i/o functions <br>
 *
 * Revision 1.3  2002/04/08 12:55:26
 * First revision 1.01 beta including :
 *  modified amino acids management
 *  new values for N & C Terminal cleavage
 *  improved statistical pre-computation of matches
 *    (for a given tag, partial matches can not have a greater "value"
 *     than complete matches)
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXSys.h"
#include "LX/LXMacros.h"

#include "PM/PMSys.h"
#include "PM/PMAa.h"


#define LOCAL_ERROR(msg, aa) sIOError(__FILE__, __LINE__, msg, aa)

static char *sCurrentLine = NULL;

/* -------------------------------------------- */
/* read error                                   */
/* -------------------------------------------- */

static int sIOError(const char *fileName, int lineno, const char *msg, const char *aa)
{
  char buffer[BUFSIZ];
  
  if (sCurrentLine) {
    strncpy(buffer, sCurrentLine, strlen(sCurrentLine) - 1);
    (void) fprintf(stderr, "// *Error* at or near line : \"%s\"\n", buffer);
  }
    
  (void) sprintf(buffer, "IO error : %s for aminoacid \"%s\"", msg, (aa ? aa : "<unknown>"));

  return LXIOError(fileName, lineno, buffer, LX_IO_ERROR);
}

/* -------------------------------------------- */
/* return Amino-Acid modification code          */
/* -------------------------------------------- */

static int sModLocation(int modloc)
{

  switch (modloc) {

    case 'N' : return PM_AA_MOD_LOC_NTERM;
    case 'C' : return PM_AA_MOD_LOC_CTERM;
    case 'X' : return PM_AA_MOD_LOC_ANY;
  }

  return -1;
}

/* -------------------------------------------- */
/* setup Amino-Acid                             */
/* -------------------------------------------- */

static void sSetupAA(PMAminoAcid *aa, int type,
        PMSymbol oneLetterCode, char *name, float mass,
        PMSymbol origin, int modLocation, float modProba) 
{
      (void) strcpy(aa->name, name);

      aa->type          = type;
      aa->oneLetterCode = oneLetterCode;
      aa->mass          = mass;
      aa->nbMod         = 0;
      aa->origin        = origin;
      aa->modLocation   = modLocation;
      aa->modProba      = modProba;
}

/* -------------------------------------------- */
/* Init modified Amino-Acid management          */
/* -------------------------------------------- */

static int sInitModifications(PMAminoAlphabet *alpha)
{
  int  i;
  
  for (i = 0 ; i < PM_MAX_AA ; i++) {

    PMAminoAcid *aa, *ori;

    if (! alpha->valid[i])               /* aa is not defined  */
      continue;

    aa = alpha->table + i;

    if (aa->type != PM_AA_MODIFIED)      /* aa is not modified */ 
      continue;
    
    /* ------------------------------------------------- */
    /* aa is modified : we should update the original aa */

    if (! alpha->valid[aa->origin])
      return LOCAL_ERROR("invalid original aa", aa->name);

    ori = alpha->table + aa->origin;

    if (ori->nbMod >= PM_MAX_MOD_PER_AA)
      return LOCAL_ERROR("too many modifications", aa->name);
    
    ori->modIndx[ori->nbMod] = i;
    ori->nbMod++;
  }

  return LX_NO_ERROR;
}

/* -------------------------------------------- */
/* IO funcs                                     */
/* -------------------------------------------- */

/* -------------------------------------------- */
/* read alphabet                                */
/* -------------------------------------------- */

int PMReadAminoAlphabet(FILE *streamin, PMAminoAlphabet *alpha) 
{
  int         code, origin, modloc;
  float       mass, proba;
  char        buffer[BUFSIZ], name[PM_AA_MAX_NAME_LEN+2];
  char        stype[8], scode[8], sorigin[8], smodloc[8];
        

  /* ------------------ */
  /* reset alphabet     */

  (void) memset(alpha, 0, sizeof(PMAminoAlphabet));

  /* ------------------ */
  /* read file          */

  while (LXGetLine(buffer, sizeof(buffer), streamin)) {

    sCurrentLine = buffer;
                                        /* ------------ */
    if (*buffer == 'N') {               /* normal aa    */
                                        /* ------------ */

      if (sscanf(buffer, "%1s%1s%s%f", 
                 stype, scode, name, &mass) != 4)
        return LOCAL_ERROR("bad input line", NULL);

      code = scode[0];

      if (! isalpha(code))
        return LOCAL_ERROR("bad one letter code", name);

      if (mass <= 0.0)
        return LOCAL_ERROR("bad mass", name);

      alpha->valid[code] = TRUE;

      sSetupAA(alpha-> table + code, PM_AA_ORIGIN,
               code, name, mass,
               -1, -1, -1); 
      
      continue;
    }

                                        /* ------------ */
    if (*buffer == 'P') {               /* pseudo aa    */
                                        /* ------------ */

      if (sscanf(buffer, "%1s%1s%s", 
                 stype, scode, name) != 3)
        return LOCAL_ERROR("bad input line", NULL);

      code = scode[0];

      if (! isalpha(code))
        return LOCAL_ERROR("bad one letter code", name);

      alpha->valid[code] = TRUE;

      sSetupAA(alpha-> table + code, PM_AA_PSEUDO,
               code, name, 0.0,
               -1, -1, -1); 
      
      continue;
    }

                                        /* ------------ */
    if (*buffer == 'M') {               /* modified aa  */
                                        /* ------------ */

      if (sscanf(buffer, "%1s%1s%s%f%1s%1s%f", 
                 stype, scode, name, &mass,
                 sorigin, smodloc, &proba) != 7)
        return LOCAL_ERROR("bad input line", NULL);

      code   = scode[0];
      origin = sorigin[0];
      modloc = sModLocation(smodloc[0]);

      if (! isalpha(code))
        return LOCAL_ERROR("bad one letter code", name);

      if (! isalpha(origin))
        return LOCAL_ERROR("bad origin code", name);
        
      if (mass < 0.0)
        return LOCAL_ERROR("bad mass", name);

      if (modloc < 0)
        return LOCAL_ERROR("bad modification location", name);

      if ((proba < 0.0) || (proba > 1.0))
        return LOCAL_ERROR("bad modification proba", name);

      alpha->valid[code] = TRUE;

      sSetupAA(alpha-> table + code, PM_AA_MODIFIED,
               code, name, mass,
               origin, modloc, proba); 

      continue;
    }

    return LOCAL_ERROR("bad AA line", NULL);       /* error        */
                                                   /* ------------ */
  }

  sCurrentLine = NULL;
  
  return sInitModifications(alpha);             /* init modifications */
}

/* -------------------------------------------- */
/* load alphabet from file                      */
/* -------------------------------------------- */

int PMLoadAminoAlphabet(char *filename, PMAminoAlphabet *alpha)
{
  FILE *alphaFile;
 
  if (! (alphaFile = LXOpenFile(filename, "r")))
    return IO_ERROR();

  int res = PMReadAminoAlphabet(alphaFile, alpha);
  
  (void) fclose(alphaFile);
  
  return res;
}

/* -------------------------------------------- */
/* get aminoacid in alphabet                    */
/* -------------------------------------------- */

PMAminoAcid *PMGetAminoAcid(PMAminoAlphabet *alpha, PMSymbol code)
{
  if ((code < 0) || (code >= PM_MAX_AA))
    return NULL;

  if (! alpha->valid[code])
    return NULL;

  return alpha->table + code;
}

/* -------------------------------------------- */
/* get full alphabet                            */
/* -------------------------------------------- */

char *PMGetFullAlphabet(PMAminoAlphabet *alpha)
{
  static char sAlpha[PM_MAX_AA];

  int i;
  char *c = sAlpha;

  for (i = 0 ; i < PM_MAX_AA ; i++) {

    if (! alpha->valid[i])
      continue;
      
    *c++ = alpha->table[i].oneLetterCode;
  }
  
  *c = '\000';
  
  return sAlpha;
}

/* -------------------------------------------- */
/* get normal alphabet                          */
/* -------------------------------------------- */

char *PMGetNormalAlphabet(PMAminoAlphabet *alpha)
{
  static char sAlpha[PM_MAX_AA];

  int i;
  char *c = sAlpha;

  for (i = 0 ; i < PM_MAX_AA ; i++) {

    if (! alpha->valid[i])
      continue;

    if (alpha->table[i].type != PM_AA_ORIGIN)
      continue;
      
    *c++ = alpha->table[i].oneLetterCode;
  }
  
  *c = '\000';
  
  return sAlpha;
}


/* -------------------------------------------- */
/* debug funcs                                  */
/* -------------------------------------------- */

/* -------------------------------------------- */

static const char *sAAType(int aaCode) {
  switch(aaCode) {
    case PM_AA_ORIGIN   : return "normal";
    case PM_AA_MODIFIED : return "modified";
    case PM_AA_PSEUDO   : return "pseudo";
  }
  
  return "<unknown>";
}

/* -------------------------------------------- */
/* print AminoAcid debug information            */
/* -------------------------------------------- */

void PMDebugAminoAcid(FILE *streamou, PMAminoAcid *aa)
{

  fprintf(streamou, "// AminoAcid\n");
  fprintf(streamou, "// name           : %s\n", aa->name);
  fprintf(streamou, "// oneLetterCode  : %c\n", aa->oneLetterCode);
  fprintf(streamou, "// type           : %s\n", sAAType(aa->type));
  fprintf(streamou, "// mass           : %f\n", aa->mass);
  if (aa->type == PM_AA_MODIFIED) {
    fprintf(streamou, "// origin         : %d (%c)\n", aa->origin, aa->origin);
    fprintf(streamou, "// modLocation    : %d\n", aa->modLocation);
    fprintf(streamou, "// modProba       : %f\n", aa->modProba);
  }
  fprintf(streamou, "//\n");
}

/* -------------------------------------------- */
/* print Alphabet debug information             */
/* -------------------------------------------- */

void PMDebugAminoAlphabet(FILE *streamou, PMAminoAlphabet *alpha)
{
  int i;

  fprintf(streamou, "// PMAminoAlphabet\n");

  for (i = 0 ; i < PM_MAX_AA ; i++) {

    if (! alpha->valid[i])
      continue;
      
    PMDebugAminoAcid(streamou, alpha->table + i);
  }
}
