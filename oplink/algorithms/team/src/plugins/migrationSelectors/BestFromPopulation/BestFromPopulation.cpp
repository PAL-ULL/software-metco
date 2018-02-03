/*********************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    AUGUST 2008
 **********************************************************************************************/
 

#include "BestFromPopulation.h"
#include "utils/utils.h"

BestFromPopulation::BestFromPopulation() : MigrationSelector(SELECT_FROM_POPULATION){
}

void BestFromPopulation::select(const vector<Individual*> &pop, int numberIndividual, vector<int> &selected){
	vector<int> nonDominated;
	markNonDominated(pop, nonDominated);
	//Select random non-dominated individuals
	int copied = 0;
	while((nonDominated.size() != 0) && (copied < numberIndividual)){
		int indexselected = generateRandom(0, nonDominated.size()-1);
		selected.push_back(nonDominated[indexselected]);
		nonDominated[indexselected] = nonDominated[nonDominated.size()-1];
		nonDominated.pop_back();
		copied++;
	}
}


