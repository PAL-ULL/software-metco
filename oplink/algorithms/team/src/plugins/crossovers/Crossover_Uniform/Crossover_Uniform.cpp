/***********************************************************************************
 * AUTHORS
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#include "Crossover_Uniform.h"

#include <math.h>

void Crossover_Uniform::crossover (Individual *ind1, Individual *ind2) {
	for (unsigned int i = 0; i < ind1->getNumberOfVar(); i++){
		if (rand() > (RAND_MAX / 2)){
			double tmp = ind2->getVar(i);
			ind2->setVar(i, ind1->getVar(i));
			ind1->setVar(i, tmp);
		} 
	}
}


