/************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    November 2008
 * **********************************************************************************/

#include "NearestObjectives.h"
#include "float.h"

void NearestObjectives::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	for (int i = 0; i < migrated.size(); i++){
		float minDist = FLT_MAX;
		int indexMin = -1;
		//Search the nearest individual (objective space)
		for (int j = 0; j < population.size(); j++){
			float dist = 0;
			for (int k = 0; k < migrated[i]->getNumberOfObj(); k++){
				dist += ( (migrated[i]->getObj(k) - population[j]->getObj(k)) * (migrated[i]->getObj(k) - population[j]->getObj(k)) );
			}
			if (dist < minDist){
				minDist = dist;
				indexMin = j;
			}
		}
		delete (population[indexMin]);
		population[indexMin] = migrated[i];
	}
}
