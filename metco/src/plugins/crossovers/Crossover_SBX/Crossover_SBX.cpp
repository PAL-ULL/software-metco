/***********************************************************************************
 * AUTHORS
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * *********************************************************************************/

#include "Crossover_SBX.h"

#include <math.h>

void Crossover_SBX::crossover (Individual *ind1, Individual *ind2) {
	const double v = 5;		//parámetro del crossover
	double u, beta, cross;

	for (unsigned int i = 0; i < ind1->getNumberOfVar(); i++){
		cross = ((double)rand())/(RAND_MAX);
		if (cross >= 0.5){
			u = ((double)rand())/(RAND_MAX + 1.0);//genera entre 0 y 1 (no incluido)
			if (u <= 0.5){//hacer un contracting crossver (beta < 1)
				beta = pow((2*u),1/(v+1.0));
			} else{//hacer un expanding crossover (beta > 1)
				beta = pow((1/(2*(1-u))), 1/(v+1.0));
			}
			ind1->setVar(i, 0.5*((1+beta)*ind1->getVar(i) + (1-beta)*ind2->getVar(i)));
			if (ind1->getVar(i) > ind1->getMaximum(i)){
				ind1->setVar(i, ind1->getMaximum(i));
			}
			if (ind1->getVar(i) < ind1->getMinimum(i)){
				ind1->setVar(i, ind1->getMinimum(i));
			}
			ind2->setVar(i, 0.5*((1-beta)*ind1->getVar(i) + (1+beta)*ind2->getVar(i)));
			if (ind2->getVar(i) > ind2->getMaximum(i)){
				ind2->setVar(i, ind2->getMaximum(i));
			}
			if (ind2->getVar(i) < ind2->getMinimum(i)){
				ind2->setVar(i, ind2->getMinimum(i));
			}
		}
	}
}


