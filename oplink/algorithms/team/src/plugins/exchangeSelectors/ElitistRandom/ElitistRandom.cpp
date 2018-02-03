/***************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    2008
 ***************************************************************************************/   
 

#include "utils.h"
#include "ElitistRandom.h"

void ElitistRandom::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	unsigned int indCopy = (migrated.size() < population.size())?migrated.size():population.size();
	//Detect dominated individuals
	vector<int> dominated;
	markDominated(population, dominated);
	//Select random dominated individuals
	int copied = 0;
	while((dominated.size() != 0) && (copied < indCopy)){
		int selected = generateRandom(0, dominated.size()-1);
		delete (population)[dominated[selected]];
		(population)[dominated[selected]] = migrated[copied];
		copied++;
		dominated[selected] = dominated[dominated.size()-1];
		dominated.pop_back();
	}
	for (unsigned int i = copied; i < migrated.size(); i++){
		delete(migrated[i]);
	}
}
