/*********************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    2012
 *
 * NOTA: Sin revisar!!!!!!!!!!!
 **********************************************************************************************/
 

#include "BestFromPopulation2.h"
#include "utils/utils.h"

BestFromPopulation2::BestFromPopulation2() : MigrationSelector(SELECT_FROM_POPULATION){
}

void BestFromPopulation2::select(const vector<Individual*> &population, int numberIndividual, vector<int> &selected){
	vector< vector <int> > ranks;
	calculateRanks(population, ranks);
	//Select random non-dominated individuals
	int copied = 0;
	int currentFront = 0;
	while(copied < numberIndividual){
		int indexSelected = generateRandom(0, ranks[currentFront].size() - 1);
		selected.push_back(ranks[currentFront][indexSelected]);
		ranks[currentFront][indexSelected] = ranks[currentFront][ranks[currentFront].size() - 1];
		ranks[currentFront].pop_back();
		copied++;
		if (ranks[currentFront].size() == 0){
			currentFront++;
			if (currentFront >= ranks.size()){
				break;
			}
		}
	}
}
