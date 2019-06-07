/*************************************************************************

 I/O functions

 ---------------------------------------------------------------------

                       Copyright (c) 2005, 2006
                  Carlos Fonseca <cmfonsec@ualg.pt>
             Manuel Lopez-Ibanez <m.lopez-ibanez@napier.ac.uk>
                    Luis Paquete <lpaquete@ualg.pt>

 This program is free software (software libre); you can redistribute
 it and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2 of the
 License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, you can obtain a copy of the GNU
 General Public License at:
                 http://www.gnu.org/copyleft/gpl.html
 or by writing to:
           Free Software Foundation, Inc., 59 Temple Place,
                 Suite 330, Boston, MA 02111-1307 USA

 ----------------------------------------------------------------------

*************************************************************************/

#include "io.h"


#define PAGE_SIZE 4096          /* allocate one page at a time      */
#define DATA_INC (PAGE_SIZE/sizeof(double))
#define SET_INC 128		/* assume 128 datasets (can grow)   */

int 
read_input(FILE *infile, const char *filename,
           double **datap, int *ncolsp, int **cumsizesp, int *nrunsp)
{
    char b[2];
    double number;

    int newline;                /* last input was newline */
    int datai;                  /* the current element of (*datap) */
    int col;                    /* the column being read */
    int set = *nrunsp;          /* the current data set */
    int ncols = *ncolsp;        /* the number of columns */

    int datasize;
    int setsize;

    datai = (set == 0) 
        ? 0 
        : ncols * (*cumsizesp)[set-1];

    setsize = (set == 0) 
        ? 0 
        : ((set-1) / SET_INC + 1) * SET_INC;

    *cumsizesp = realloc(*cumsizesp, setsize * sizeof(int));

    datasize = (datai == 0) 
        ? 0 
        : ((datai - 1) / DATA_INC + 1) * DATA_INC;

    *datap = realloc(*datap, datasize * sizeof(double));

    /* remove leading whitespace */
    fscanf (infile, "%*[ \t\n]");

    if (feof(infile)) {
        warnprintf("%s: file is empty.", filename);
        return -1;
    }
    
    do {
        newline = 0;
        if (set == setsize) {
            setsize += SET_INC;
            *cumsizesp = realloc(*cumsizesp,  setsize * sizeof(int));
        }

        (*cumsizesp)[set] = (set == 0) ? 0 : (*cumsizesp)[set-1];       /* beginning of data set */

        while (newline == 0) {
	    col = 0;		/* beginning of row */
            while (newline == 0) {
		if (fscanf(infile, "%lf", &number) != 1) {
                    char buffer[64];

                    fscanf(infile, "%60s", buffer);
                    errprintf(
                        "could not convert string `%s' to double, exiting...", 
                        buffer);
                }

                if (datai == datasize) {
                    datasize += DATA_INC;
                    *datap = realloc(*datap,  datasize * sizeof(double));
                }
                (*datap)[datai++] = number;
                
#if DEBUG > 2
                fprintf(stderr, "set %d, row %d, column %d, x = %g\n", 
                        set, (*cumsizesp)[set], col, number);
#endif
                col++;    /* new column */
                
                fscanf(infile, "%*[ \t]");
                newline = fscanf(infile, "%1[\n]", b); 
            }

            if (!ncols) 
                ncols = col;
            else if (col != ncols) {
                if ((*cumsizesp)[0] == 0) 
                    errprintf ("reference point has dimension %d"
                               " while input has dimension %d",
                               ncols, col);
                else
                    errprintf("row %d has different length (%d)"
                              " than previous rows (%d), exiting...",
                              (*cumsizesp)[set], col, ncols);
            }

            (*cumsizesp)[set]++;
	
            fscanf (infile, "%*[ \t]");	
	    newline = fscanf (infile, "%1[\n]", b); 
	}
#if DEBUG > 1
        fprintf (stderr, "Set %d, %d rows in total\n", set, (*cumsizesp)[set]);
#endif
	set++; /* new data set */
	
        fscanf (infile, "%*[ \t\n]");

    } while (!feof(infile)); 

    *ncolsp = ncols;
    *nrunsp = set;

    return 0;
}



/* From:

   Edition 0.10, last updated 2001-07-06, of `The GNU C Library
   Reference Manual', for Version 2.3.x.

   Copyright (C) 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2001, 2002,
   2003 Free Software Foundation, Inc.
*/
void errprintf(const char *template,...)
{
    extern char *program_invocation_short_name;
    va_list ap;

    fprintf(stderr, "%s: error: ", program_invocation_short_name);
    va_start(ap,template);
    vfprintf(stderr, template, ap);
    va_end(ap);
    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
}
/* End of copyright The GNU C Library Reference Manual */

void warnprintf(const char *template,...)
{
    extern char *program_invocation_short_name;
    va_list ap;

    fprintf(stderr, "%s: warning: ", program_invocation_short_name);
    va_start(ap,template);
    vfprintf(stderr, template, ap);
    va_end(ap);
    fprintf(stderr, "\n");

}
