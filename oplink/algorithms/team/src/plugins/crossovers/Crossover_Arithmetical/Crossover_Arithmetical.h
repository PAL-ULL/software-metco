/***********************************************************************************
 * AUTHORS
 *   - Carlos Segura González
 *
 * Paper: Michalewicz, Evolutionary algorithms for constrained engineering problems.
 * *********************************************************************************/


#ifndef CROSSOVER_ARITHMETICAL_H
#define CROSSOVER_ARITHMETICAL_H

#include "Crossover.h"

class Crossover_Arithmetical : public Crossover {
	void crossover(Individual *ind1, Individual *ind2);
};

#endif
