/* -----------------------------------------------------------------------
 * $Id: LXCompl.c 1791 2013-02-24 02:17:56Z viari $
 * -----------------------------------------------------------------------
 * @file: LXComplPtr.c
 * @desc: job completion notifier
 * 
 * @history:
 * @+ <Gloup> : Mar 10 : first version
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * job completion notifier<br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "LX/LXSys.h"
#include "LX/LXCompl.h"

#define BUFFER_SIZE_    1024

#define SCREEN_WIDTH_   80

/* ==================================================== */
/* internal macros                                      */
/* ==================================================== */

// -----------------
// output macros
// -----------------

#define OUT_(fmt, args...)          \
        fprintf(stderr, fmt, ## args)

#define PUT(b)                      \
        OUT_("%s", b);

#define OUTL_(b, c, p)              \
        OUT_("%c%s%c\n", c, sPad(b, p, SCREEN_WIDTH_-2), c)

#define LINE                        \
        OUTL_("", '+', '-')

#define OUT(b)                      \
        OUTL_(b, '|', ' ')

// -----------------
// buffer mngt macros & generic CSI
// -----------------

#define INIT(b)                     \
        *(b) = '\000'

#define APPEND(b, fmt, args...)     \
        (void) sprintf(b + strlen(b), fmt, ## args)

#define APC_(b, c) APPEND(b, "%c", c)
#define APS_(b, s) APPEND(b, "%s", s)
#define APN_(b, n) APPEND(b, "%d", n)

#define CSI_(b) APS_(b, "\033[")
#define CSIC_(b, c)     CSI_(b); APC_(b, c)
#define CSIS_(b, s, c)  CSI_(b); APS_(b, s);  APC_(b, c)
#define CSIN_(b, n, c)  CSI_(b); APN_(b, n);  APC_(b, c)

// -----------------
// cursor mngt macros
// -----------------

#define NUP(b, n)    CSIN_(b, n, 'A')
#define NDOWN(b, n)  CSIN_(b, n, 'B')
#define HOME(b)      CSIN_(b, 1, 'H')
#define CLEAR(b)     CSIN_(b, 2, 'J')
#define SAVE(b)      CSIC_(b, 's')      // or '7'
#define RESTORE(b)   CSIC_(b, 'u')      // or '8'

#define SCROLL(b, n) CSI_(b); APN_(b, n); APC_(b, ';'); APC_(b, 'r')

// -----------------
// SGR macros
// -----------------

#define SGR_(b, n)   CSIN_(b, n, 'm')

#define NORMAL(b)    SGR_(b, 0)

#define BOLD(b)      SGR_(b, 1)
#define BLINK(b)     SGR_(b, 5)
#define INVERT(b)    SGR_(b, 7)

#define F_BLACK(b)   SGR_(b, 30)
#define F_RED(b)     SGR_(b, 31)
#define F_GREEN(b)   SGR_(b, 32)
#define F_YELLOW(b)  SGR_(b, 33)
#define F_BLUE(b)    SGR_(b, 34)
#define F_MAGENTA(b) SGR_(b, 35)
#define F_CYAN(b)    SGR_(b, 36)
#define F_WHITE(b)   SGR_(b, 37)

#define B_BLACK(b)   SGR_(b, 40)
#define B_RED(b)     SGR_(b, 41)
#define B_GREEN(b)   SGR_(b, 42)
#define B_YELLOW(b)  SGR_(b, 43)
#define B_BLUE(b)    SGR_(b, 44)
#define B_MAGENTA(b) SGR_(b, 45)
#define B_CYAN(b)    SGR_(b, 46)
#define B_WHITE(b)   SGR_(b, 47)

// -----------------
// shortcuts macros
// -----------------

#define ANSI_TITLE(b, s)   INIT(b); F_BLUE(b); APPEND(buf, s); NORMAL(b)
#define TEXT_TITLE(b, s)   INIT(b); APPEND(buf, s)

/* ==================================================== */
/* internal structures                                  */
/* ==================================================== */

/* ---------------------------- */

typedef struct {
  int hour;
  int min;
  double sec;
} sTime;

/* ---------------------------- */

typedef struct {
  float mem;
  char units[8];
} sMem;

/* ---------------------------- */

typedef struct {
  void (*init)(LXComplPtr comp);
  void (*display)(LXComplPtr comp);
  void (*end)(LXComplPtr comp);
} sHandler;

/* ---------------------------- */

// actual completer structure
                          /* ---------------------------- */
struct LXCompl_Struct {   /* internal completer           */
                          /* ---------------------------- */
                          /* user defined                 */
                          /* ---------------------------- */
  char        name[64];   /* name                         */
  char        units[32];  /* units                        */
  size_t      nmax;       /* max value                    */
  double      deltamin;   /* min delta time (ms)          */
  char      **userinfo;   /* optional user info           */
  sHandler    handler;    /* output handlers              */
                          /* ---------------------------- */
                          /* internally computed          */
                          /* ---------------------------- */
  size_t      curr;       /* - current value              */
  double      percent;    /* - percentage completion      */
  int         nuserinfo;  /* - # items in userinfo        */
  double      startime;   /* - start time                 */
  double      lastime;    /* - last recorded time         */
  double      startcpu;   /* - start cpu time             */
  double      percpu;     /* - % cpu used                 */
  double      speed;      /* - speed : items / sec        */
  Bool        atstart;    /* - state : at start           */
  Bool        atdisplay;  /* - state : at display         */
  Bool        atfinish;   /* - state : at end             */
  Bool        hasmax;     /* - has maximum value          */
  int         modulo;     /* - current modulo value       */
  sTime       s_elapsed;  /* - elapsed time struct        */
  sTime       s_remain;   /* - remaining time struct      */
  sTime       s_cpu;      /* - cpu time struct            */
  size_t      memused;    /* - memory used (bytes)        */
  size_t      memfree;    /* - memory free (bytes)        */
  sMem        s_memused;  /* - memory used struct         */
  sMem        s_memfree;  /* - memory free struct         */
  int         barlen;     /* - length of completion bar   */
};

/* ==================================================== */
/* internal handlers                                    */
/* ==================================================== */

static void sNone(LXComplPtr comp);

static void sSimpleInit(LXComplPtr comp);
static void sSimpleEnd(LXComplPtr comp);
static void sSimpleDisplay(LXComplPtr comp);

static void sTextInit(LXComplPtr comp);
static void sTextEnd(LXComplPtr comp);
static void sTextDisplay(LXComplPtr comp);

static void sAnsiInit(LXComplPtr comp);
static void sAnsiEnd(LXComplPtr comp);
static void sAnsiDisplay(LXComplPtr comp);

static sHandler sNoneHandler   = {sNone, sNone, sNone};
static sHandler sSimpleHandler = {sSimpleInit, sSimpleDisplay, sSimpleEnd};
static sHandler sTextHandler   = {sTextInit, sTextDisplay, sTextEnd};
static sHandler sAnsiHandler   = {sAnsiInit, sAnsiDisplay, sAnsiEnd};

/* ==================================================== */
/* internal functions                                   */
/* ==================================================== */

/* ---------------------------- */

static int sLen(char *s) {
    int n;
    for (n = 0 ; *s ; s++) {
      if (*s == '\033') { // escape CSI
        while (*s && (! isalpha(*s)))
            s++;
      }
      else {
        n++;
      }
    }
    return n;
}

/* ---------------------------- */

static char *sPad(const char *s, int padchar, int len) {
    static char sBuffer[BUFFER_SIZE_];
    (void) strcpy(sBuffer, (s ? s : ""));
    int i = sLen(sBuffer);
    int j = strlen(sBuffer);
    while (i++ < len) {
      sBuffer[j++] = padchar;
    }
    sBuffer[j] = '\000';
    return sBuffer;
}

/* ---------------------------- */
static char *sBar(int barchar, int len) {
    static char sBuffer[BUFFER_SIZE_];
    int i;
    for (i = 0 ; i < len ; i++)
      sBuffer[i] = barchar;
    sBuffer[len] = '\000';
    return sBuffer;
}

/* ---------------------------- */

static void sGetSTime(double tim, sTime *stim) {
    stim->hour = floor(tim / 3600.);
    tim -= stim->hour * 3600.;
    stim->min  = floor(tim / 60.);
    tim -= stim->min * 60.;
    stim->sec  = tim;
}

/* ---------------------------- */

static const char *sMemUnits[5] = {"b", "kb", "Mb", "Gb", "Tb"};

static void sGetXMemory(size_t mem, sMem *smem) {
    float x = (float) mem;
    int i;
    for (i = 0 ; i < 4 ; i++) {
      x /= 1000.;
      if (x < 1.) {
        smem->mem = x * 1000.;
        (void) strcpy(smem->units, sMemUnits[i]);
        return;
      }
    }
    smem->mem = x;
    (void) strcpy(smem->units, sMemUnits[4]);
}

/* ---------------------------- */

static int sGetNbUserInfo(char **userinfo) {
  int n = 0;
  while (userinfo && *userinfo++) {
      n++;
  }
  return n;
}

/* ---------------------------- */

static sHandler sGetHandler(LXComplMode mode) {

  //
  // todo : in auto mode we should determine terminal
  // capabilities
  //
  if (mode == LX_COMPLETER_AUTO)
    mode = LX_COMPLETER_ANSI;

  switch (mode) {
    case LX_COMPLETER_ANSI :
        return sAnsiHandler;
    case LX_COMPLETER_SIMPLE :
        return sSimpleHandler;
    case LX_COMPLETER_TEXT :
        return sTextHandler;
    default:
        return sNoneHandler;
  }
}

/* -------------------------------------------- */

static int sUpdateCompleter(LXComplPtr comp, size_t curr) {

    double now    = LXCurrentTime();
    double nowcpu = LXUserCpuTime(LX_TIME_NO_RESET);
    double delta  = now - comp->lastime;

    comp->curr   = curr;
    comp->hasmax = (comp->nmax != 0);

    comp->atstart   = (comp->startime == 0);
    comp->atfinish  = comp->atfinish || (comp->hasmax && (curr >= comp->nmax));
    comp->atdisplay = comp->atstart || comp->atfinish || (delta >= comp->deltamin);

    if (! comp->atdisplay)      // maybe next time
        return LX_NO_ERROR; 

    if (comp->startime == 0) {
        comp->startime = now;
        comp->startcpu = nowcpu;
    }
       
    comp->percent = (  comp->hasmax 
                     ? (double) curr / (double) comp->nmax
                     : (double) comp->modulo / (double) SCREEN_WIDTH_);

    double pastime = now - comp->startime;
    double pastcpu = nowcpu - comp->startcpu;
    
    sGetSTime(pastime, &comp->s_elapsed);

    sGetSTime(pastime * (1. - comp->percent), &comp->s_remain);
    
    sGetSTime(pastcpu, &comp->s_cpu);

    comp->percpu = (pastime ? pastcpu / pastime : 1.);

    comp->speed = (pastime ? (double) curr / pastime : 0.);

    comp->memused = LXGetUsedMemory();
    comp->memfree = 0;

    sGetXMemory(comp->memused, &comp->s_memused);

    sGetXMemory(comp->memfree, &comp->s_memfree);

    comp->barlen = comp->percent * (SCREEN_WIDTH_-2);

    comp->nuserinfo = sGetNbUserInfo(comp->userinfo);

    comp->lastime = now;

    comp->modulo = (comp->modulo + 1) % SCREEN_WIDTH_;
    
    return LX_NO_ERROR;
}

/* ==================================================== */
/* internal output handlers                             */
/* ==================================================== */

/* -------------------------------------------- */
/* None mode handlers                           */
/* -------------------------------------------- */

static void sNone(LXComplPtr comp) {
    (void) comp;
}

/* -------------------------------------------- */
/* Simple mode handlers                         */
/* -------------------------------------------- */

static void sSimpleInit(LXComplPtr comp) {
    (void) comp;
}

static void sSimpleEnd(LXComplPtr comp) {
    (void) comp;
    OUT_("\n");
}

static void sSimpleDisplay(LXComplPtr comp) {

    char buf[BUFFER_SIZE_];
    
    static char sRotator[] = "-/|\\";
    
    INIT(buf);                  // completion bar
    if (comp->hasmax) {
       APPEND(buf, "%s", sPad(sBar('=', comp->barlen), '.', SCREEN_WIDTH_-2));
       APPEND(buf, "| %zu %s / %zu (%.0f%%)", comp->curr, comp->units, 
                                             comp->nmax, comp->percent * 100.);
    }
    else {
       int irot = comp->modulo % (sizeof(sRotator)-1);
       APPEND(buf, "%c", sRotator[irot]);
       APPEND(buf, " %zu %s done", comp->curr, comp->units); 
    }
    OUT_("\r%s\r", buf);

}

/* -------------------------------------------- */
/* Text mode handlers                           */
/* -------------------------------------------- */

static void sTextInit(LXComplPtr comp) {
    (void) comp;
}

static void sTextEnd(LXComplPtr comp) {
    (void) comp;
}

static void sTextDisplay(LXComplPtr comp) {

    char buf[BUFFER_SIZE_];

    LINE;

    TEXT_TITLE(buf, " ");
    if (comp->atfinish) {
      APPEND(buf, " %s <Finished> ", comp->name);
    }
    else {
      APPEND(buf, " %s <Running> ", comp->name);
    }
    OUT(buf);


    TEXT_TITLE(buf, " completion:");
    APPEND(buf, " %s", comp->units);
    APPEND(buf, " %zu", comp->curr);
    if (comp->hasmax) {
      APPEND(buf, " / %zu", comp->nmax);
      APPEND(buf, " (%.0f %%)", comp->percent * 100.);
    }
    OUT(buf);
  
    TEXT_TITLE(buf, " time:");
    APPEND(buf, " elapsed %2.2d:%2.2d:%05.2f", comp->s_elapsed.hour, comp->s_elapsed.min, comp->s_elapsed.sec);
    if (comp->hasmax) {
      APPEND(buf, " remain %2.2d:%2.2d:%05.2f", comp->s_remain.hour, comp->s_remain.min, comp->s_remain.sec);
    }
    APPEND(buf, " / CPU %2.2d:%2.2d:%05.2f", comp->s_cpu.hour, comp->s_cpu.min, comp->s_cpu.sec);
    APPEND(buf, " (%.0f %%)", comp->percpu * 100.);
    OUT(buf);

    TEXT_TITLE(buf, " speed:");
    APPEND(buf, " %.1f %s/s", comp->speed, comp->units);
    OUT(buf);

    TEXT_TITLE(buf, " memory:");
    APPEND(buf, " used %zu (%.1f %s)", comp->memused, comp->s_memused.mem, comp->s_memused.units);
    APPEND(buf, " free %zu (%.1f %s)", comp->memfree, comp->s_memfree.mem, comp->s_memfree.units);
    OUT(buf);
  
    LINE;
    
    if (comp->nuserinfo) {
      int i;
      for (i = 0 ; i < comp->nuserinfo ; i++) {
         INIT(buf);
         APPEND(buf, " %s", comp->userinfo[i]);
         OUT(buf);
      }
      LINE;
    }
}

/* -------------------------------------------- */
/* ANSI mode handlers                           */
/* -------------------------------------------- */

static void sAnsiInit(LXComplPtr comp) {
    char buf[BUFFER_SIZE_];

    int nlines = comp->nuserinfo;
    nlines = 10 + (nlines ? nlines + 1 : 0);
    
    INIT(buf);
    SAVE(buf);
    SCROLL(buf, nlines);    
    HOME(buf);
    PUT(buf);
}

/* ---------------------------- */

static void sAnsiEnd(LXComplPtr comp) {
    char buf[BUFFER_SIZE_];
    (void) comp;
    
    INIT(buf);
    SCROLL(buf, 1);      
    RESTORE(buf);
    PUT(buf);
}

/* ---------------------------- */

static void sAnsiDisplay(LXComplPtr comp) {

    char buf[BUFFER_SIZE_];

    INIT(buf);
    HOME(buf);
    PUT(buf);

    LINE;

    ANSI_TITLE(buf, " ");
    INVERT(buf);
    if (comp->atfinish) {
      F_RED(buf);
      APPEND(buf, " %s <Finished> ", comp->name);
    }
    else {
      F_GREEN(buf);
      APPEND(buf, " %s <Running> ", comp->name);
    }
    NORMAL(buf);
    OUT(buf);


    ANSI_TITLE(buf, " completion:");
    APPEND(buf, " %s", comp->units);
    APPEND(buf, " %zu", comp->curr);
    if (comp->hasmax) {
      APPEND(buf, " / %zu", comp->nmax);
      APPEND(buf, " (%.0f %%)", comp->percent * 100.);
    }
    OUT(buf);
  
    ANSI_TITLE(buf, " time:");
    APPEND(buf, " elapsed %2.2d:%2.2d:%05.2f", comp->s_elapsed.hour, comp->s_elapsed.min, comp->s_elapsed.sec);
    if (comp->hasmax) {
      APPEND(buf, " remain %2.2d:%2.2d:%05.2f", comp->s_remain.hour, comp->s_remain.min, comp->s_remain.sec);
    }
    APPEND(buf, " / CPU %2.2d:%2.2d:%05.2f", comp->s_cpu.hour, comp->s_cpu.min, comp->s_cpu.sec);
    APPEND(buf, " (%.0f %%)", comp->percpu * 100.);
    OUT(buf);

    ANSI_TITLE(buf, " speed:");
    APPEND(buf, " %.1f %s/s", comp->speed, comp->units);
    OUT(buf);

    ANSI_TITLE(buf, " memory:");
    APPEND(buf, " used %zu (%.1f %s)", comp->memused, comp->s_memused.mem, comp->s_memused.units);
    APPEND(buf, " free %zu (%.1f %s)", comp->memfree, comp->s_memfree.mem, comp->s_memfree.units);
    OUT(buf);
  
    LINE;
    
    INIT(buf);                  // completion bar
    if (comp->hasmax) {
       B_BLUE(buf);
       APPEND(buf, "%s", sBar('-', comp->barlen));
    }
    else {
       APPEND(buf, "%s", sBar(' ', comp->barlen));
       B_BLUE(buf);
       APPEND(buf, "-");
    }
    NORMAL(buf);
    OUT(buf);

    LINE;

    if (comp->nuserinfo) {
      int i;
      for (i = 0 ; i < comp->nuserinfo ; i++) {
         INIT(buf);
         APPEND(buf, " %s", comp->userinfo[i]);
         OUT(buf);
      }
      LINE;
    }
    
    INIT(buf);
    NORMAL(buf);
    RESTORE(buf);
    PUT(buf);
}

/* ==================================================== */
/* API                                                  */
/* ==================================================== */

/* -------------------------------------------- */

LXComplPtr LXFreeCompleter(LXComplPtr comp) {
    IFFREE(comp);
    return NULL;
}

/* ---------------------------- */

LXComplPtr LXNewCompleter(LXComplMode mode, char *name, size_t nmax,
                          char *units, double deltamin, char *userinfo[]) {
                            
    LXComplPtr comp = NEW(struct LXCompl_Struct);
    IFF_RETURN(comp, NULL);

    (void) memset(comp, 0, sizeof(*comp));

    comp->handler  = sGetHandler(mode);
    
    comp->nmax     = nmax;
    comp->deltamin = deltamin / 1000.; // millisec
    comp->userinfo = userinfo;
    comp->startime = 0;
    comp->atfinish = FALSE;

    (void) strncpy(comp->name, (name ? name : ""), sizeof(comp->name)-1);
    (void) strncpy(comp->units, (units ? units : ""), sizeof(comp->units)-1);
    
    return comp;
}

/* -------------------------------------------- */

int LXDoCompleter(LXComplPtr comp, size_t curr) {
    IFF_RETURN(comp, LX_ARG_ERROR);

    sUpdateCompleter(comp, curr);

    if (comp->atstart)
        comp->handler.init(comp);
    
    if (comp->atdisplay)
        comp->handler.display(comp);
        
    if (comp->atfinish)
        comp->handler.end(comp);
        
    return LX_NO_ERROR;
}

/* -------------------------------------------- */

int LXEndCompleter(LXComplPtr comp) {
    IFF_RETURN(comp, LX_ARG_ERROR);
    comp->atfinish = TRUE;
    return LXDoCompleter(comp, comp->curr);
}

/* -------------------------------------------- */

int LXResetCompleter(LXComplPtr comp) {
    IFF_RETURN(comp, LX_ARG_ERROR);
    comp->startime = 0;
    comp->atfinish = FALSE;
    return LX_NO_ERROR;
}
