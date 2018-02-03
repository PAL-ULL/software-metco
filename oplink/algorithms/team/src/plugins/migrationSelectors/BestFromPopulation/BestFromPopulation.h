/*********************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    AUGUST 2008
 *
 * DESCRIPTION
 *    It selects non-dominated individuals from the population
 *********************************************************************************************/

#ifndef BEST_FROM_POPULATION_H
#define BEST_FROM_POPULATION_H

#include "Individual.h"
#include "MigrationSelector.h"

#include <vector>

using namespace std;

class BestFromPopulation : public MigrationSelector {
	public:
		BestFromPopulation();
		void select(const vector<Individual*> &pop, int numberIndividual, vector<int> &selected);
};

#endif
