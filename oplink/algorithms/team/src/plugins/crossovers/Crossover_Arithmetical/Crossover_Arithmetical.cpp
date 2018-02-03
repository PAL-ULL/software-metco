/***********************************************************************************
 * AUTHORS
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#include "Crossover_Arithmetical.h"
#include "utils.h"

#include <math.h>

void Crossover_Arithmetical::crossover (Individual *ind1, Individual *ind2) {
	double w = generateRandomDouble0_Max(1);
	for (unsigned int i = 0; i < ind1->getNumberOfVar(); i++){
		double tmp1 = ind1->getVar(i);
		double tmp2 = ind2->getVar(i);
		ind2->setVar(i, w * tmp1 + (1 - w) * tmp2);
		ind1->setVar(i, w * tmp2 + (1 - w) * tmp1);
	}
}


