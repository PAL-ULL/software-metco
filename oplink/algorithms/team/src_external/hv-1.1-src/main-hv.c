/*************************************************************************

 hv: main program

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
#include "hv.h"
#include "timer.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h> // for isspace()

#include <unistd.h>  // for getopt()
#include <getopt.h> // for getopt_long()

#if !defined(TRUE) || !defined(FALSE)
#define TRUE 1
#define FALSE 0
#endif

#if VARIANT < 4
int stop_dimension = 1; // default: stop on dimension 2
#else
int stop_dimension = 2; // default: stop on dimension 3
#endif


extern char *program_invocation_short_name;

double * read_reference(char * str, int *nobj)
{
    double * reference;
    char * endp;
    char * cursor;

    int k = 0, size = 10;

    reference = malloc(size * sizeof(double));
    endp = str;

    do {
        cursor = endp;
        if (k == size) {
            size += 10;
            reference = realloc(reference, size * sizeof(double));
        }
        reference[k] = strtod(cursor, &endp);
        k++;
    } while (cursor != endp);

    // not end of string: error
    while (*cursor != '\0') {
        if (!isspace(*cursor)) return NULL;
        cursor++;
    }

    // no number: error
    if (k == 1) return NULL;

    *nobj = k-1;
    return reference;
}

void usage(void)
{
    printf("\n"
           "Usage: %s [OPTIONS] [FILE...]\n\n", program_invocation_short_name);

    printf(
"Calculate the hypervolume of the union set of all input FILEs. \n"
"With no FILE, or when FILE is -, read standard input.\n\n"

"Options:\n"
" -h, --help          print this summary and exit.                          \n"
"     --version       print version number and exit.                        \n"
" -v, --verbose       print some information (time, input points, output    \n"
"                     points, etc). Default is --quiet.                     \n"
" -q, --quiet         print just the hypervolume (as opposed to --verbose). \n"
" -r, --reference=POINT use POINT as reference point. POINT must be within  \n"
"                     quotes, e.g., \"10 10 10\". If no reference point is  \n"
"                     given, it is taken as the maximum value for each      \n"
"                     coordinate from the input points.                     \n"
" -1, --stop-on-1D    stop recursion in dimension 1                         \n"
" -2, --stop-on-2D    stop recursion in dimension 2    %s\n"
" -3, --stop-on-3D    stop recursion in dimension 3    %s\n"
"\n",
(stop_dimension == 1) ? ("(default)") : (""),
(stop_dimension == 2) ? ("(default)") : ("") );

}

void version(void)
{
    printf("%s version %s (optimised for %s)\n\n",
           program_invocation_short_name, VERSION, MARCH);
    printf(
"Copyright (C) 2006  \nCarlos Fonseca (cmfonsec@ualg.pt), "
"\nManuel Lopez-Ibanez (m.lopez-ibanez@napier.ac.uk) and "
"\nLuis Paquete (lpaquete@ualg.pt)\n"
"\n"
"This is free software, and you are welcome to redistribute it under certain\n"
"conditions.  See the GNU General Public License for details. There is NO   \n"
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
"\n"        );
}

double * data_maximum(double *data, int nobj, int rows)
{
    double *vector;
    int n, r, k;

    vector = malloc(nobj*sizeof(double));
    
    for (k = 0; k < nobj; k++) 
        vector[k] = data[k];

    for (r = 1 ; r < rows; r++) {
        for (n = 0; n < nobj; n++, k++) {
            if (vector[n] < data[k] )
                vector[n] = data[k];
        }
    }

    return vector;
}

int main(int argc, char *argv[])
{
    FILE *infile;

    int verbose_flag = FALSE;


    double *data = NULL, *reference = NULL, *maximum = NULL;
    int *cumsizes = NULL;
    int nobj = 0, nruns = 0;
    int k;
    double volume = 0;
    double time_elapsed;


    int opt; /* it's actually going to hold a char */
    int longopt_index;

    /* see the man page for getopt_long for an explanation of these fields */
    static struct option long_options[] = {
        {"help",       no_argument,       NULL, 'h'},
        {"version",    no_argument,       NULL, 'V'},
        {"verbose",    no_argument,       NULL, 'v'},
        {"quiet",      no_argument,       NULL, 'q'},
        {"reference",  required_argument, NULL, 'r'},
        {"stop-on-1D", no_argument,       NULL, '1'},
        {"stop-on-2D", no_argument,       NULL, '2'},
        {"stop-on-3D", no_argument,       NULL, '3'},
        {NULL, 0, NULL, 0} /* marks end of list */
    };

    while (0 < (opt = getopt_long(argc, argv, "hVvqr:123",
                                  long_options, &longopt_index))) {
        switch (opt) {
        case '1':
            stop_dimension = 0; break;

        case '2':
            stop_dimension = 1; break;

        case '3':
            stop_dimension = 2; break;

        case 'r': // --reference
            reference = read_reference(optarg, &nobj);
            if (reference == NULL) {
                fprintf(stderr, "%s: invalid reference point '%s'\n",
                        program_invocation_short_name,optarg);
                exit(EXIT_FAILURE);
            }
            break;

        case 'V': // --version
            version();
            exit(EXIT_SUCCESS);

        case 'q': // --quiet
            verbose_flag = FALSE;
            break;

        case 'v': // --verbose
            verbose_flag = TRUE;
            break;

        case '?':
            // getopt prints an error message right here
            fprintf(stderr, "Try `%s --help' for more information.\n",
                    program_invocation_short_name);
            exit(EXIT_FAILURE);
        case 'h':
            usage();
            exit(EXIT_SUCCESS);

        default: // should never happen
            abort();
        }
    }

    if (optind < argc)
	for (; optind < argc; optind++)
	    if (strncmp(argv[optind],"-", strlen("-")+1)) {
		infile = fopen(argv[optind],"r");
                if (infile == NULL) {
                    fprintf (stderr, "%s: %s: %s\n",
                             program_invocation_short_name,
                             argv[optind], strerror(errno));
                    exit(EXIT_FAILURE);
                }
		read_input(infile, argv[optind], 
                           &data, &nobj, &cumsizes, &nruns);
		fclose(infile);
	    } else
		read_input(stdin, "stdin", &data, &nobj, &cumsizes, &nruns);
    else
	read_input(stdin, "stdin", &data, &nobj, &cumsizes, &nruns);

    if (cumsizes[0] == 0) 
        errprintf ("no input data.");
    
    maximum = data_maximum(data, nobj, cumsizes[nruns-1]);

    if (reference == NULL) {
        reference=calloc(nobj, sizeof(double));
        memcpy(reference, maximum, nobj*sizeof(double));
    }


    Timer_start();
    volume =  hv(data, nobj, cumsizes[nruns-1], reference);
    time_elapsed = Timer_elapsed(VIRTUAL_TIME);

    printf("%f\n", volume);

    if (verbose_flag) {

        printf("Time: %f seconds\n", time_elapsed);

        printf("maximum:");
        for(k = 0; k < nobj; k++) {
            printf(" %f", maximum[k]);
        }
        printf("\n");

        printf("reference:");
        for(k = 0; k < nobj; k++) {
            printf(" %f", reference[k]);
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
}
