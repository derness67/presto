/*****
  command line parser -- generated by clig
  (http://wsd.iitb.fhg.de/~kir/clighome/)

  The command line parser `clig':
  (C) 1995---2001 Harald Kirsch (kirschh@lionbioscience.com)
*****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include "readfile_cmd.h"

char *Program;

/*@-null*/
static int indexDefault[] = {0,  -1};

static Cmdline cmd = {
  /***** -nopage: Don't paginate the output like 'more' */
  /* nopageP = */ 0,
  /***** -byte: Raw data in byte format */
  /* bytP = */ 0,
  /***** -b: Raw data in byte format */
  /* sbytP = */ 0,
  /***** -float: Raw data in floating point format */
  /* fltP = */ 0,
  /***** -f: Raw data in floating point format */
  /* sfltP = */ 0,
  /***** -double: Raw data in double precision format */
  /* dblP = */ 0,
  /***** -d: Raw data in double precision format */
  /* sdblP = */ 0,
  /***** -fcomplex: Raw data in float-complex format */
  /* fcxP = */ 0,
  /***** -fc: Raw data in float-complex format */
  /* sfcxP = */ 0,
  /***** -dcomplex: Raw data in double-complex format */
  /* dcxP = */ 0,
  /***** -dc: Raw data in double-complex format */
  /* sdcxP = */ 0,
  /***** -short: Raw data in short format */
  /* shtP = */ 0,
  /***** -s: Raw data in short format */
  /* sshtP = */ 0,
  /***** -int: Raw data in integer format */
  /* igrP = */ 0,
  /***** -i: Raw data in integer format */
  /* sigrP = */ 0,
  /***** -long: Raw data in long format */
  /* lngP = */ 0,
  /***** -l: Raw data in long format */
  /* slngP = */ 0,
  /***** -rzwcand: Raw data in rzw search candidate format */
  /* rzwP = */ 0,
  /***** -rzw: Raw data in rzw search candidate format */
  /* srzwP = */ 0,
  /***** -bincand: Raw data in bin search candidate format */
  /* binP = */ 0,
  /***** -bin: Raw data in bin search candidate format */
  /* sbinP = */ 0,
  /***** -position: Raw data in position struct format */
  /* posP = */ 0,
  /***** -pos: Raw data in position struct format */
  /* sposP = */ 0,
  /***** -pkmb: Raw data in Parkes Multibeam format */
  /* pksP = */ 0,
  /***** -pk: Raw data in Parkes Multibeam format */
  /* spksP = */ 0,
  /***** -bpp: Raw data in BPP format */
  /* bppP = */ 0,
  /***** -wapp: Raw data in WAPP format */
  /* wappP = */ 0,
  /***** -fortran: Raw data was written by a fortran program */
  /* fortranP = */ 0,
  /***** -index: The range of objects to display */
  /* indexP = */ 1,
  /* index = */ indexDefault,
  /* indexC = */ 2,
  /***** -nph: 0th FFT bin amplitude (for 'RZW' data) */
  /* nphP = */ 1,
  /* nph = */ 1.0,
  /* nphC = */ 1,
  /***** uninterpreted rest of command line */
  /* argc = */ 0,
  /* argv = */ (char**)0,
  /***** the original command line concatenated */
  /* full_cmd_line = */ NULL
};

/*@=null*/

/***** let LCLint run more smoothly */
/*@-predboolothers*/
/*@-boolops*/


/******************************************************************/
/*****
 This is a bit tricky. We want to make a difference between overflow
 and underflow and we want to allow v==Inf or v==-Inf but not
 v>FLT_MAX. 

 We don't use fabs to avoid linkage with -lm.
*****/
static void
checkFloatConversion(double v, char *option, char *arg)
{
  char *err = NULL;

  if( (errno==ERANGE && v!=0.0) /* even double overflowed */
      || (v<HUGE_VAL && v>-HUGE_VAL && (v<0.0?-v:v)>(double)FLT_MAX) ) {
    err = "large";
  } else if( (errno==ERANGE && v==0.0) 
	     || (v!=0.0 && (v<0.0?-v:v)<(double)FLT_MIN) ) {
    err = "small";
  }
  if( err ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to %s to represent\n",
	    Program, arg, option, err);
    exit(EXIT_FAILURE);
  }
}

int
getIntOpt(int argc, char **argv, int i, int *value, int force)
{
  char *end;
  long v;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  v = strtol(argv[i], &end, 0);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check if it fits into an int */
  if( errno==ERANGE || v>(long)INT_MAX || v<(long)INT_MIN ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to large to represent\n",
	    Program, argv[i], argv[i-1]);
    exit(EXIT_FAILURE);
  }
  *value = (int)v;

  return i;

nothingFound:
  if( !force ) return i-1;

  fprintf(stderr, 
	  "%s: missing or malformed integer value after option `%s'\n",
	  Program, argv[i-1]);
    exit(EXIT_FAILURE);
}
/**********************************************************************/

int
getIntOpts(int argc, char **argv, int i, 
	   int **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;
  long v;
  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values. It does not hurt to have room
    for a bit more values than cmax.
  *****/
  alloced = cmin + 4;
  *values = (int*)calloc((size_t)alloced, sizeof(int));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (int *) realloc(*values, alloced*sizeof(int));
      if( !*values ) goto outMem;
    }

    errno = 0;
    v = strtol(argv[used+i+1], &end, 0);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1;
    if( *end ) break;

    /***** check for overflow */
    if( errno==ERANGE || v>(long)INT_MAX || v<(long)INT_MIN ) {
      fprintf(stderr, 
	      "%s: parameter `%s' of option `%s' to large to represent\n",
	      Program, argv[i+used+1], argv[i]);
      exit(EXIT_FAILURE);
    }

    (*values)[used] = (int)v;

  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be an "
	    "integer value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

int
getLongOpt(int argc, char **argv, int i, long *value, int force)
{
  char *end;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  *value = strtol(argv[i], &end, 0);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check for overflow */
  if( errno==ERANGE ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to large to represent\n",
	    Program, argv[i], argv[i-1]);
    exit(EXIT_FAILURE);
  }
  return i;

nothingFound:
  /***** !force means: this parameter may be missing.*/
  if( !force ) return i-1;

  fprintf(stderr, 
	  "%s: missing or malformed value after option `%s'\n",
	  Program, argv[i-1]);
    exit(EXIT_FAILURE);
}
/**********************************************************************/

int
getLongOpts(int argc, char **argv, int i, 
	    long **values,
	    int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values. It does not hurt to have room
    for a bit more values than cmax.
  *****/
  alloced = cmin + 4;
  *values = calloc((size_t)alloced, sizeof(long));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = realloc(*values, alloced*sizeof(long));
      if( !*values ) goto outMem;
    }

    errno = 0;
    (*values)[used] = strtol(argv[used+i+1], &end, 0);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1; 
    if( *end ) break;

    /***** check for overflow */
    if( errno==ERANGE ) {
      fprintf(stderr, 
	      "%s: parameter `%s' of option `%s' to large to represent\n",
	      Program, argv[i+used+1], argv[i]);
      exit(EXIT_FAILURE);
    }

  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be an "
	    "integer value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

int
getFloatOpt(int argc, char **argv, int i, float *value, int force)
{
  char *end;
  double v;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  v = strtod(argv[i], &end);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check for overflow */
  checkFloatConversion(v, argv[i-1], argv[i]);

  *value = (float)v;

  return i;

nothingFound:
  if( !force ) return i-1;

  fprintf(stderr,
	  "%s: missing or malformed float value after option `%s'\n",
	  Program, argv[i-1]);
  exit(EXIT_FAILURE);
 
}
/**********************************************************************/

int
getFloatOpts(int argc, char **argv, int i, 
	   float **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;
  double v;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values.
  *****/
  alloced = cmin + 4;
  *values = (float*)calloc((size_t)alloced, sizeof(float));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (float *) realloc(*values, alloced*sizeof(float));
      if( !*values ) goto outMem;
    }

    errno = 0;
    v = strtod(argv[used+i+1], &end);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1;
    if( *end ) break;

    /***** check for overflow */
    checkFloatConversion(v, argv[i], argv[i+used+1]);
    
    (*values)[used] = (float)v;
  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be a "
	    "floating-point value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

int
getDoubleOpt(int argc, char **argv, int i, double *value, int force)
{
  char *end;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  *value = strtod(argv[i], &end);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check for overflow */
  if( errno==ERANGE ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to %s to represent\n",
	    Program, argv[i], argv[i-1],
	    (*value==0.0 ? "small" : "large"));
    exit(EXIT_FAILURE);
  }

  return i;

nothingFound:
  if( !force ) return i-1;

  fprintf(stderr,
	  "%s: missing or malformed value after option `%s'\n",
	  Program, argv[i-1]);
  exit(EXIT_FAILURE);
 
}
/**********************************************************************/

int
getDoubleOpts(int argc, char **argv, int i, 
	   double **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values.
  *****/
  alloced = cmin + 4;
  *values = (double*)calloc((size_t)alloced, sizeof(double));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (double *) realloc(*values, alloced*sizeof(double));
      if( !*values ) goto outMem;
    }

    errno = 0;
    (*values)[used] = strtod(argv[used+i+1], &end);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1;
    if( *end ) break;

    /***** check for overflow */
    if( errno==ERANGE ) {
      fprintf(stderr, 
	      "%s: parameter `%s' of option `%s' to %s to represent\n",
	      Program, argv[i+used+1], argv[i],
	      ((*values)[used]==0.0 ? "small" : "large"));
      exit(EXIT_FAILURE);
    }

  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be a "
	    "double value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

/**
  force will be set if we need at least one argument for the option.
*****/
int
getStringOpt(int argc, char **argv, int i, char **value, int force)
{
  i += 1;
  if( i>=argc ) {
    if( force ) {
      fprintf(stderr, "%s: missing string after option `%s'\n",
	      Program, argv[i-1]);
      exit(EXIT_FAILURE);
    } 
    return i-1;
  }
  
  if( !force && argv[i][0] == '-' ) return i-1;
  *value = argv[i];
  return i;
}
/**********************************************************************/

int
getStringOpts(int argc, char **argv, int i, 
	   char*  **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  alloced = cmin + 4;
    
  *values = (char**)calloc((size_t)alloced, sizeof(char*));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory during parsing of option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (char **)realloc(*values, alloced*sizeof(char*));
      if( !*values ) goto outMem;
    }

    if( used>=cmin && argv[used+i+1][0]=='-' ) break;
    (*values)[used] = argv[used+i+1];
  }
    
  if( used<cmin ) {
    fprintf(stderr, 
    "%s: less than %d parameters for option `%s', only %d found\n",
	    Program, cmin, argv[i], used);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

void
checkIntLower(char *opt, int *values, int count, int max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%d\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkIntHigher(char *opt, int *values, int count, int min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%d\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkLongLower(char *opt, long *values, int count, long max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%ld\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkLongHigher(char *opt, long *values, int count, long min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%ld\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkFloatLower(char *opt, float *values, int count, float max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%f\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkFloatHigher(char *opt, float *values, int count, float min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%f\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkDoubleLower(char *opt, double *values, int count, double max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%f\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkDoubleHigher(char *opt, double *values, int count, double min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%f\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

static char *
catArgv(int argc, char **argv)
{
  int i;
  size_t l;
  char *s, *t;

  for(i=0, l=0; i<argc; i++) l += (1+strlen(argv[i]));
  s = (char *)malloc(l);
  if( !s ) {
    fprintf(stderr, "%s: out of memory\n", Program);
    exit(EXIT_FAILURE);
  }
  strcpy(s, argv[0]);
  t = s;
  for(i=1; i<argc; i++) {
    t = t+strlen(t);
    *t++ = ' ';
    strcpy(t, argv[i]);
  }
  return s;
}
/**********************************************************************/

void
showOptionValues(void)
{
  int i;

  printf("Full command line is:\n`%s'\n", cmd.full_cmd_line);

  /***** -nopage: Don't paginate the output like 'more' */
  if( !cmd.nopageP ) {
    printf("-nopage not found.\n");
  } else {
    printf("-nopage found:\n");
  }

  /***** -byte: Raw data in byte format */
  if( !cmd.bytP ) {
    printf("-byte not found.\n");
  } else {
    printf("-byte found:\n");
  }

  /***** -b: Raw data in byte format */
  if( !cmd.sbytP ) {
    printf("-b not found.\n");
  } else {
    printf("-b found:\n");
  }

  /***** -float: Raw data in floating point format */
  if( !cmd.fltP ) {
    printf("-float not found.\n");
  } else {
    printf("-float found:\n");
  }

  /***** -f: Raw data in floating point format */
  if( !cmd.sfltP ) {
    printf("-f not found.\n");
  } else {
    printf("-f found:\n");
  }

  /***** -double: Raw data in double precision format */
  if( !cmd.dblP ) {
    printf("-double not found.\n");
  } else {
    printf("-double found:\n");
  }

  /***** -d: Raw data in double precision format */
  if( !cmd.sdblP ) {
    printf("-d not found.\n");
  } else {
    printf("-d found:\n");
  }

  /***** -fcomplex: Raw data in float-complex format */
  if( !cmd.fcxP ) {
    printf("-fcomplex not found.\n");
  } else {
    printf("-fcomplex found:\n");
  }

  /***** -fc: Raw data in float-complex format */
  if( !cmd.sfcxP ) {
    printf("-fc not found.\n");
  } else {
    printf("-fc found:\n");
  }

  /***** -dcomplex: Raw data in double-complex format */
  if( !cmd.dcxP ) {
    printf("-dcomplex not found.\n");
  } else {
    printf("-dcomplex found:\n");
  }

  /***** -dc: Raw data in double-complex format */
  if( !cmd.sdcxP ) {
    printf("-dc not found.\n");
  } else {
    printf("-dc found:\n");
  }

  /***** -short: Raw data in short format */
  if( !cmd.shtP ) {
    printf("-short not found.\n");
  } else {
    printf("-short found:\n");
  }

  /***** -s: Raw data in short format */
  if( !cmd.sshtP ) {
    printf("-s not found.\n");
  } else {
    printf("-s found:\n");
  }

  /***** -int: Raw data in integer format */
  if( !cmd.igrP ) {
    printf("-int not found.\n");
  } else {
    printf("-int found:\n");
  }

  /***** -i: Raw data in integer format */
  if( !cmd.sigrP ) {
    printf("-i not found.\n");
  } else {
    printf("-i found:\n");
  }

  /***** -long: Raw data in long format */
  if( !cmd.lngP ) {
    printf("-long not found.\n");
  } else {
    printf("-long found:\n");
  }

  /***** -l: Raw data in long format */
  if( !cmd.slngP ) {
    printf("-l not found.\n");
  } else {
    printf("-l found:\n");
  }

  /***** -rzwcand: Raw data in rzw search candidate format */
  if( !cmd.rzwP ) {
    printf("-rzwcand not found.\n");
  } else {
    printf("-rzwcand found:\n");
  }

  /***** -rzw: Raw data in rzw search candidate format */
  if( !cmd.srzwP ) {
    printf("-rzw not found.\n");
  } else {
    printf("-rzw found:\n");
  }

  /***** -bincand: Raw data in bin search candidate format */
  if( !cmd.binP ) {
    printf("-bincand not found.\n");
  } else {
    printf("-bincand found:\n");
  }

  /***** -bin: Raw data in bin search candidate format */
  if( !cmd.sbinP ) {
    printf("-bin not found.\n");
  } else {
    printf("-bin found:\n");
  }

  /***** -position: Raw data in position struct format */
  if( !cmd.posP ) {
    printf("-position not found.\n");
  } else {
    printf("-position found:\n");
  }

  /***** -pos: Raw data in position struct format */
  if( !cmd.sposP ) {
    printf("-pos not found.\n");
  } else {
    printf("-pos found:\n");
  }

  /***** -pkmb: Raw data in Parkes Multibeam format */
  if( !cmd.pksP ) {
    printf("-pkmb not found.\n");
  } else {
    printf("-pkmb found:\n");
  }

  /***** -pk: Raw data in Parkes Multibeam format */
  if( !cmd.spksP ) {
    printf("-pk not found.\n");
  } else {
    printf("-pk found:\n");
  }

  /***** -bpp: Raw data in BPP format */
  if( !cmd.bppP ) {
    printf("-bpp not found.\n");
  } else {
    printf("-bpp found:\n");
  }

  /***** -wapp: Raw data in WAPP format */
  if( !cmd.wappP ) {
    printf("-wapp not found.\n");
  } else {
    printf("-wapp found:\n");
  }

  /***** -fortran: Raw data was written by a fortran program */
  if( !cmd.fortranP ) {
    printf("-fortran not found.\n");
  } else {
    printf("-fortran found:\n");
  }

  /***** -index: The range of objects to display */
  if( !cmd.indexP ) {
    printf("-index not found.\n");
  } else {
    printf("-index found:\n");
    if( !cmd.indexC ) {
      printf("  no values\n");
    } else {
      printf("  values =");
      for(i=0; i<cmd.indexC; i++) {
        printf(" `%d'", cmd.index[i]);
      }
      printf("\n");
    }
  }

  /***** -nph: 0th FFT bin amplitude (for 'RZW' data) */
  if( !cmd.nphP ) {
    printf("-nph not found.\n");
  } else {
    printf("-nph found:\n");
    if( !cmd.nphC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.nph);
    }
  }
  if( !cmd.argc ) {
    printf("no remaining parameters in argv\n");
  } else {
    printf("argv =");
    for(i=0; i<cmd.argc; i++) {
      printf(" `%s'", cmd.argv[i]);
    }
    printf("\n");
  }
}
/**********************************************************************/

void
usage(void)
{
  fprintf(stderr, "usage: %s%s", Program, "\
 [-nopage] [-byte] [-b] [-float] [-f] [-double] [-d] [-fcomplex] [-fc] [-dcomplex] [-dc] [-short] [-s] [-int] [-i] [-long] [-l] [-rzwcand] [-rzw] [-bincand] [-bin] [-position] [-pos] [-pkmb] [-pk] [-bpp] [-wapp] [-fortran] [-index [index]] [-nph nph] [--] file\n\
    Reads raw data from a binary file and displays it on stdout.\n\
    -nopage: Don't paginate the output like 'more'\n\
      -byte: Raw data in byte format\n\
         -b: Raw data in byte format\n\
     -float: Raw data in floating point format\n\
         -f: Raw data in floating point format\n\
    -double: Raw data in double precision format\n\
         -d: Raw data in double precision format\n\
  -fcomplex: Raw data in float-complex format\n\
        -fc: Raw data in float-complex format\n\
  -dcomplex: Raw data in double-complex format\n\
        -dc: Raw data in double-complex format\n\
     -short: Raw data in short format\n\
         -s: Raw data in short format\n\
       -int: Raw data in integer format\n\
         -i: Raw data in integer format\n\
      -long: Raw data in long format\n\
         -l: Raw data in long format\n\
   -rzwcand: Raw data in rzw search candidate format\n\
       -rzw: Raw data in rzw search candidate format\n\
   -bincand: Raw data in bin search candidate format\n\
       -bin: Raw data in bin search candidate format\n\
  -position: Raw data in position struct format\n\
       -pos: Raw data in position struct format\n\
      -pkmb: Raw data in Parkes Multibeam format\n\
        -pk: Raw data in Parkes Multibeam format\n\
       -bpp: Raw data in BPP format\n\
      -wapp: Raw data in WAPP format\n\
   -fortran: Raw data was written by a fortran program\n\
     -index: The range of objects to display\n\
             0...2 int values between -1 and oo\n\
             default: `0' ` -1'\n\
       -nph: 0th FFT bin amplitude (for 'RZW' data)\n\
             1 double value\n\
             default: `1.0'\n\
       file: Input data file name.\n\
             1 value\n\
version: 12Sep02\n\
");
  exit(EXIT_FAILURE);
}
/**********************************************************************/
Cmdline *
parseCmdline(int argc, char **argv)
{
  int i;

  Program = argv[0];
  cmd.full_cmd_line = catArgv(argc, argv);
  for(i=1, cmd.argc=1; i<argc; i++) {
    if( 0==strcmp("--", argv[i]) ) {
      while( ++i<argc ) argv[cmd.argc++] = argv[i];
      continue;
    }

    if( 0==strcmp("-nopage", argv[i]) ) {
      cmd.nopageP = 1;
      continue;
    }

    if( 0==strcmp("-byte", argv[i]) ) {
      cmd.bytP = 1;
      continue;
    }

    if( 0==strcmp("-b", argv[i]) ) {
      cmd.sbytP = 1;
      continue;
    }

    if( 0==strcmp("-float", argv[i]) ) {
      cmd.fltP = 1;
      continue;
    }

    if( 0==strcmp("-f", argv[i]) ) {
      cmd.sfltP = 1;
      continue;
    }

    if( 0==strcmp("-double", argv[i]) ) {
      cmd.dblP = 1;
      continue;
    }

    if( 0==strcmp("-d", argv[i]) ) {
      cmd.sdblP = 1;
      continue;
    }

    if( 0==strcmp("-fcomplex", argv[i]) ) {
      cmd.fcxP = 1;
      continue;
    }

    if( 0==strcmp("-fc", argv[i]) ) {
      cmd.sfcxP = 1;
      continue;
    }

    if( 0==strcmp("-dcomplex", argv[i]) ) {
      cmd.dcxP = 1;
      continue;
    }

    if( 0==strcmp("-dc", argv[i]) ) {
      cmd.sdcxP = 1;
      continue;
    }

    if( 0==strcmp("-short", argv[i]) ) {
      cmd.shtP = 1;
      continue;
    }

    if( 0==strcmp("-s", argv[i]) ) {
      cmd.sshtP = 1;
      continue;
    }

    if( 0==strcmp("-int", argv[i]) ) {
      cmd.igrP = 1;
      continue;
    }

    if( 0==strcmp("-i", argv[i]) ) {
      cmd.sigrP = 1;
      continue;
    }

    if( 0==strcmp("-long", argv[i]) ) {
      cmd.lngP = 1;
      continue;
    }

    if( 0==strcmp("-l", argv[i]) ) {
      cmd.slngP = 1;
      continue;
    }

    if( 0==strcmp("-rzwcand", argv[i]) ) {
      cmd.rzwP = 1;
      continue;
    }

    if( 0==strcmp("-rzw", argv[i]) ) {
      cmd.srzwP = 1;
      continue;
    }

    if( 0==strcmp("-bincand", argv[i]) ) {
      cmd.binP = 1;
      continue;
    }

    if( 0==strcmp("-bin", argv[i]) ) {
      cmd.sbinP = 1;
      continue;
    }

    if( 0==strcmp("-position", argv[i]) ) {
      cmd.posP = 1;
      continue;
    }

    if( 0==strcmp("-pos", argv[i]) ) {
      cmd.sposP = 1;
      continue;
    }

    if( 0==strcmp("-pkmb", argv[i]) ) {
      cmd.pksP = 1;
      continue;
    }

    if( 0==strcmp("-pk", argv[i]) ) {
      cmd.spksP = 1;
      continue;
    }

    if( 0==strcmp("-bpp", argv[i]) ) {
      cmd.bppP = 1;
      continue;
    }

    if( 0==strcmp("-wapp", argv[i]) ) {
      cmd.wappP = 1;
      continue;
    }

    if( 0==strcmp("-fortran", argv[i]) ) {
      cmd.fortranP = 1;
      continue;
    }

    if( 0==strcmp("-index", argv[i]) ) {
      int keep = i;
      cmd.indexP = 1;
      i = getIntOpts(argc, argv, i, &cmd.index, 0, 2);
      cmd.indexC = i-keep;
      checkIntHigher("-index", cmd.index, cmd.indexC, -1);
      continue;
    }

    if( 0==strcmp("-nph", argv[i]) ) {
      int keep = i;
      cmd.nphP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.nph, 1);
      cmd.nphC = i-keep;
      continue;
    }

    if( argv[i][0]=='-' ) {
      fprintf(stderr, "\n%s: unknown option `%s'\n\n",
              Program, argv[i]);
      usage();
    }
    argv[cmd.argc++] = argv[i];
  }/* for i */


  /*@-mustfree*/
  cmd.argv = argv+1;
  /*@=mustfree*/
  cmd.argc -= 1;

  if( 1>cmd.argc ) {
    fprintf(stderr, "%s: there should be at least 1 non-option argument(s)\n",
            Program);
    exit(EXIT_FAILURE);
  }
  if( 1<cmd.argc ) {
    fprintf(stderr, "%s: there should be at most 1 non-option argument(s)\n",
            Program);
    exit(EXIT_FAILURE);
  }
  /*@-compmempass*/  return &cmd;
}

