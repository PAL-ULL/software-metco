/************************************************************************
 * AUTHORS
 *    Gara Miranda Valladares
 *    Eduardo Segredo González
 *    Carlos Segura Gonzalez
 *
 * DATE
 *    July 2007
 **********************************************************************/
 

#include "Mutate_BinaryFlip_All.h"

void Mutate_BinaryFlip_All::mutation(Individual *ind, const double pm){
	for (unsigned int i = 0; i < ind->getNumberOfVar(); i++) {
		double vmut = ((double) rand()) / (double) (RAND_MAX);
		if (vmut < pm) {
			if (ind->getVar(i) <= 0.5)
				ind->setVar(i, 1);
			else
				ind->setVar(i, 0);
		}
	}
}

