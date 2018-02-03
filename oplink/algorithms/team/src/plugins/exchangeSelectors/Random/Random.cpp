/*********************************************************************************
 * AUTHORS
 *    Carlos Segura Gonzalez
 *
 * DATE
 *    November 2008
 *********************************************************************************/
 
#include "Random.h"
#include "utils.h"

void Random::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	unsigned int indCopy = (migrated.size() < population.size())?migrated.size():population.size();
	for (unsigned int i = 0; i < indCopy; i++){
		int selected = generateRandom(0, population.size()-1);
		delete (population)[selected];
		(population)[selected] = migrated[i];
	}
	for (unsigned int i = indCopy; i < migrated.size(); i++){
		delete(migrated[i]);
	}
}
