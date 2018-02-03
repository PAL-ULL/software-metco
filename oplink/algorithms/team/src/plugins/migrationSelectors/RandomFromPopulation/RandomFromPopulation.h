/*******************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    August 2008
 *
 * DESCRIPTION
 *    It selects random individuals from the current population 
 *******************************************************************************************/


#ifndef RANDOM_FROM_POPULATION_H
#define RANDOM_FROM_POPULATION_H

#include "Individual.h"
#include "MigrationSelector.h"

#include <vector>

using namespace std;

class RandomFromPopulation : public MigrationSelector {
	public:
		RandomFromPopulation();
		void select(const vector<Individual*> &pop, int numberIndividual, vector<int> &selected);
};

#endif
