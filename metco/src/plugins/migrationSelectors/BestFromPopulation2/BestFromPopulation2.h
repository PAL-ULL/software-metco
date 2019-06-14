/*********************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    AUGUST 2008
 *
 * DESCRIPTION
 *    It selects some of the best individuals 
 *********************************************************************************************/

#ifndef BEST_FROM_POPULATION_H
#define BEST_FROM_POPULATION_H

#include "Individual.h"
#include "MigrationSelector.h"

#include <vector>

using namespace std;

class BestFromPopulation2 : public MigrationSelector {
	public:
		BestFromPopulation2();
		void select(const vector<Individual*> &pop, int numberIndividual, vector<int> &selected);
};

#endif
