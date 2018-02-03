#include "Mutate_Polynomial2.h"

#include <math.h>

void Mutate_Polynomial2::mutation(Individual *ind, const double pm){
	const double distributionIndex = 20.0 ;
	double delta1, delta2, deltaq, mut_pow, rnd, tmp;

	for (int i = 0; i < ind->getNumberOfVar(); i++) {
		if ((double)rand()/RAND_MAX <= pm) {
			delta1 = (ind->getVar(i) - ind->getMinimum(i)) / (ind->getMaximum(i) - ind->getMinimum(i)) ;
			delta2 = (ind->getMaximum(i) - ind->getVar(i)) / (ind->getMaximum(i) - ind->getMinimum(i)) ;
			mut_pow = 1.0/(distributionIndex + 1.0) ;
			rnd = (double)rand()/RAND_MAX ;
			if (rnd <= 0.5) {
				tmp = 2.0 *rnd + (1.0-2.0 * rnd) * pow(1.0-delta1, distributionIndex + 1.0) ;
				deltaq = pow(tmp, mut_pow) - 1.0 ;
			} // if
			else {
				tmp = 2.0*(1.0-rnd) + 2.0*(rnd-0.5)*
				      pow(1.0-delta2,distributionIndex+1.0) ;
				deltaq = 1.0 - pow(tmp, mut_pow) ;					
			} // else
			ind->setVar(i, ind->getVar(i) + deltaq*(ind->getMaximum(i)-ind->getMinimum(i))) ;
			if (ind->getVar(i) < ind->getMinimum(i))
				ind->setVar(i, ind->getMinimum(i)) ;
			if (ind->getVar(i) > ind->getMaximum(i))
				ind->setVar(i, ind->getMaximum(i)); 
		} // if
	} // for
}

