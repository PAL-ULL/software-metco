/************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    November 2008
 * **********************************************************************************/


#include "RandomIfDom.h"
#include "utils.h"

void RandomIfDom::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	unsigned int indCopy = (migrated.size() < population.size())?migrated.size():population.size();
	for (unsigned int i = 0; i < indCopy; i++){
		int selected = generateRandom(0, population.size()-1);
		if (dominanceTest(population[selected], migrated[i]) == SECOND_DOMINATES){
			delete (population)[selected];
			(population)[selected] = migrated[i];
		} else {
			delete migrated[i];
		}
	}
	for (unsigned int i = indCopy; i < migrated.size(); i++){
		delete(migrated[i]);
	}
}
