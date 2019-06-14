/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo
 *   - Carlos Segura
 * 
 * *********************************************************************************/

#include "Crossover_PBX_Alpha.h"

#include <math.h>

void Crossover_PBX_Alpha::crossover (Individual *ind1, Individual *ind2) {
  // Parámetro alpha del crossover
	const double alpha = 0.5;

	for (unsigned int i = 0; i < ind1->getNumberOfVar(); i++) {
    double I = fabs(ind1->getVar(i) - ind2->getVar(i));

    double min_ind1 = fmax(ind1->getMinimum(i), ind1->getVar(i) - (alpha * I));
    double max_ind1 = fmin(ind1->getMaximum(i), ind1->getVar(i) + (alpha * I));

    double min_ind2 = fmax(ind2->getMinimum(i), ind2->getVar(i) - (alpha * I));
    double max_ind2 = fmin(ind2->getMaximum(i), ind2->getVar(i) + (alpha * I));

    ind1->setVar(i, generateRandomDoubleMinMax(min_ind1, max_ind1));
    ind2->setVar(i, generateRandomDoubleMinMax(min_ind2, max_ind2));

	}
}
