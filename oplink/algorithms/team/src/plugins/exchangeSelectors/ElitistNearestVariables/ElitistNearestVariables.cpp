#include "ElitistNearestVariables.h"
#include "utils.h"

#include <float.h>
void ElitistNearestVariables::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	unsigned int indCopy = (migrated.size() < population.size())?migrated.size():population.size();
	//Detect dominated individuals
	vector<int> dominated;
	markDominated(population, dominated);
	//The nearest dominated individual (variables space) is replaced
	int copied = 0;
	while((dominated.size() != 0) && (copied < indCopy)){
		float minDist = FLT_MAX;
		int selected = -1;
		for (int j = 0; j < dominated.size(); j++){
			float dist = 0;
			for (int k = 0; k < migrated[copied]->getNumberOfVar(); k++){
				dist += ( (migrated[copied]->getVar(k) - population[dominated[j]]->getVar(k)) * (migrated[copied]->getVar(k) - population[dominated[j]]->getVar(k)) );
			}
			if (dist < minDist){
				minDist = dist;
				selected = dominated[j];
			}
		}
		delete (population)[selected];
		(population)[selected] = migrated[copied];
		copied++;
		dominated[selected] = dominated[dominated.size()-1];
		dominated.pop_back();
	}
	for (unsigned int i = copied; i < migrated.size(); i++){
		delete(migrated[i]);
	}
}
