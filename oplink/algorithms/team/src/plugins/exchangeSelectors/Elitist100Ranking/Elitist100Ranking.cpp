/*****************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE 
 *    March 2009
 *****************************************************************************************************/
 
#include <algorithm>
#include "Elitist100Ranking.h"
#include "utils.h"

void Elitist100Ranking::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	for (int i = 0; i < migrated.size(); i++)
		population.push_back(migrated[i]);
	vector< vector <int> > ranks;
	calculateRanks(population, ranks);
	vector<int> indexToRemove;
	//Remove elements from the worst ranks
	for (int i = 0; i < migrated.size(); i++){
		//Select individual to remove 
		int rnd = generateRandom(0, ranks[ranks.size()-1].size() - 1);
		indexToRemove.push_back(ranks[ranks.size()-1][rnd]);

		//Erase used
		ranks[ranks.size()-1][rnd] = ranks[ranks.size()-1][ranks[ranks.size()-1].size() - 1];
		ranks[ranks.size()-1].pop_back();
		if (ranks[ranks.size()-1].size() == 0)
			ranks.pop_back();
	}

	//Erase the worst individuals
	sort(indexToRemove.begin(), indexToRemove.end());
	for (int i = indexToRemove.size() - 1; i >= 0; i--){
		delete (population[indexToRemove[i]]);
		population[indexToRemove[i]] = population[population.size()-1];
		population.pop_back();
	}
}
