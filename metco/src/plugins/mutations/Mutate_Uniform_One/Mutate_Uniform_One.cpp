/*************************************************************************
 * AUTHORS
 *    Gara Miranda Valladares
 *    Carlos Segura Gonzalez
 *************************************************************************/


#include "Mutate_Uniform_One.h"

void Mutate_Uniform_One::mutation(Individual *ind, const double pm){
	double vmut = ((double) rand()) / (double) (RAND_MAX);
	if (vmut < pm) {
		int indElem = (int) ((double)(ind->getNumberOfVar()) * rand() / (RAND_MAX + 1.0));
		double tmp = ((double) rand () / (double) RAND_MAX);
		ind->setVar(indElem, tmp * (ind->getMaximum(indElem) - ind->getMinimum(indElem)) + ind->getMinimum(indElem));
	}
}

