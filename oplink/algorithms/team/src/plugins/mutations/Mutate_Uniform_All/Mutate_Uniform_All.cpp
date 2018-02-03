/*************************************************************************
 * AUTHORS
 *    Gara Miranda Valladares
 *    Carlos Segura Gonzalez
 *************************************************************************/

#include "Mutate_Uniform_All.h"

void Mutate_Uniform_All::mutation(Individual *ind, const double pm){
	for (unsigned int i = 0; i < ind->getNumberOfVar(); i++) {
		float vmut = ((double) rand()) / (double) (RAND_MAX);
		if (vmut < pm) {
			double tmp = ((double) rand () / (double) RAND_MAX);
			ind->setVar(i, tmp * (ind->getMaximum(i) - ind->getMinimum(i)) + ind->getMinimum(i)); 
		}
	}
}

