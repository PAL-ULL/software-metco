/*******************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    August 2008
 *******************************************************************************************/


#include "RandomFromPopulation.h"
#include "utils/utils.h"

RandomFromPopulation::RandomFromPopulation() : MigrationSelector(SELECT_FROM_POPULATION){
}

void RandomFromPopulation::select(const vector<Individual*> &pop, int numberIndividual, vector<int> &selected){
	int generatedNumbers = (pop.size() > numberIndividual)?numberIndividual:pop.size();
	generateRandomValues(0, pop.size()-1, generatedNumbers, selected);
}


