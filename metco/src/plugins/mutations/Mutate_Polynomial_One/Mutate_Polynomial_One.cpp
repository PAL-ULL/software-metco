#include "Mutate_Polynomial_One.h"

#include <math.h>

void Mutate_Polynomial_One::mutation(Individual *ind, const double pm) {
	const double distributionIndex = 20.0;
	double delta1, delta2, deltaq, mut_pow, rnd, tmp;

	if ((double)rand()/RAND_MAX <= pm) {
    int indElem = (int)((double)(ind->getNumberOfVar()) * rand() / (RAND_MAX + 1.0));
		delta1 = (ind->getVar(indElem) - ind->getMinimum(indElem)) / (ind->getMaximum(indElem) - ind->getMinimum(indElem));
		delta2 = (ind->getMaximum(indElem) - ind->getVar(indElem)) / (ind->getMaximum(indElem) - ind->getMinimum(indElem));
		mut_pow = 1.0/(distributionIndex + 1.0);
		rnd = (double)rand()/RAND_MAX;

		if (rnd <= 0.5) {
			tmp = 2.0 * rnd + (1.0 - 2.0 * rnd) * pow(1.0 - delta1, distributionIndex + 1.0);
			deltaq = pow(tmp, mut_pow) - 1.0 ;
		} // if
		else {
			tmp = 2.0 * (1.0 - rnd) + 2.0 * (rnd - 0.5) * pow(1.0 - delta2, distributionIndex + 1.0);
			deltaq = 1.0 - pow(tmp, mut_pow) ;
		} // else

		ind->setVar(indElem, ind->getVar(indElem) + deltaq * (ind->getMaximum(indElem) - ind->getMinimum(indElem)));

		if (ind->getVar(indElem) < ind->getMinimum(indElem))
			ind->setVar(indElem, ind->getMinimum(indElem)) ;
		if (ind->getVar(indElem) > ind->getMaximum(indElem))
			ind->setVar(indElem, ind->getMaximum(indElem)); 
	} // if
}
