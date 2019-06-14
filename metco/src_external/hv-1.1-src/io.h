#ifndef _HV_IO_H_
#define _HV_IO_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


/* If we're not using GNU C, elide __attribute__ */
#ifndef __GNUC__
#  define  __attribute__(x)  /* NOTHING */
#endif

void 
errprintf(const char * template,...) 
/* enables the compiler to check the format string against the
   parameters */  __attribute__ ((format(printf, 1, 2)));

void warnprintf(const char *template,...)
/* enables the compiler to check the format string against the
   parameters */  __attribute__ ((format(printf, 1, 2)));

int 
read_input(FILE *infile, const char *filename, 
           double **datap, int *ncolsp, int **cumsizesp, int *nrunsp);

#endif
