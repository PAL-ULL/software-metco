/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo
 *   - Carlos Segura
 * 
 * *********************************************************************************/

#include "Crossover_BLX_Alpha.h"

#include <math.h>

void Crossover_BLX_Alpha::crossover (Individual *ind1, Individual *ind2) {
  // Parámetro alpha del crossover
	const double alpha = 0.5;

	for (unsigned int i = 0; i < ind1->getNumberOfVar(); i++) {
    double d = fabs(ind1->getVar(i) - ind2->getVar(i));

    double min = fmin(ind1->getVar(i), ind2->getVar(i)) - (alpha * d);
    double max = fmax(ind1->getVar(i), ind2->getVar(i)) + (alpha * d);

    ind1->setVar(i, generateRandomDoubleMinMax(min, max));
    if (ind1->getVar(i) > ind1->getMaximum(i)) {
      ind1->setVar(i, ind1->getMaximum(i));
    }
    if (ind1->getVar(i) < ind1->getMinimum(i)) {
      ind1->setVar(i, ind1->getMinimum(i));
    }
   
    ind2->setVar(i, generateRandomDoubleMinMax(min, max));
    if (ind2->getVar(i) > ind2->getMaximum(i)) {
      ind2->setVar(i, ind2->getMaximum(i));
    }
    if (ind2->getVar(i) < ind2->getMinimum(i)) {
      ind2->setVar(i, ind2->getMinimum(i));
    }
	}
}
