/*****************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE 
 *    March 2009
 *****************************************************************************************************/
 

#include "ElitistRanking.h"
#include "utils.h"

void ElitistRanking::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	unsigned int indCopy = (migrated.size() < population.size())?migrated.size():population.size();
	vector< vector <int> > ranks;
	calculateRanks(population, ranks);
	//Remove elements from the worst ranks
	for (int i = 0; i < indCopy; i++){
		//Select individual to exchange
		int rnd = generateRandom(0, ranks[ranks.size()-1].size() - 1);
		int index = ranks[ranks.size()-1][rnd];

		//Exchange
		delete(population[index]);
		population[index] = migrated[i];

		//Erase used
		ranks[ranks.size()-1][rnd] = ranks[ranks.size()-1][ranks[ranks.size()-1].size() - 1];
		ranks[ranks.size()-1].pop_back();
		if (ranks[ranks.size()-1].size() == 0)
			ranks.pop_back();
	}
	for (unsigned int i = indCopy; i < migrated.size(); i++){
		delete(migrated[i]);
	}
}
