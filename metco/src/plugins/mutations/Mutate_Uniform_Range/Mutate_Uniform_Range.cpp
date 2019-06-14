/*************************************************************************
 * AUTHORS
 *    Gara Miranda Valladares
 *    Carlos Segura Gonzalez
 *************************************************************************/

#include "Mutate_Uniform_Range.h"

void Mutate_Uniform_Range::mutation(Individual *ind, const double pm){
	// Generate interval to mutate
	int start = (int) ((double)(ind->getNumberOfVar())*rand()/(RAND_MAX+1.0));
	int end = (int) ((double)(ind->getNumberOfVar())*rand()/(RAND_MAX+1.0));
	int to;
	int starts[2], ends[2];
	if (end < start){
		starts[0] = 0;
		ends[0] = end;
		starts[1] = start;
		ends[1] = ind->getNumberOfVar() - 1;
		to = 2;
	} else {
		starts[0] = start;
		ends[0] = end;
		to = 1;
	}
	for (int i = 0; i < to; i++){
		for (int j = starts[i]; j <= ends[i]; j++){
			double vmut = ((double) rand()) / (double) (RAND_MAX);
			if (vmut < pm) {
				double tmp = ((double) rand () / (double) RAND_MAX);
				ind->setVar(j, tmp * (ind->getMaximum(j) - ind->getMinimum(j)) + ind->getMinimum(j));
			}
		}
	}
}

