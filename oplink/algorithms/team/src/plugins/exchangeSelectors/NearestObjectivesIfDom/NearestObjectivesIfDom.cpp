/************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    November 2008
 ***********************************************************************************/

#include "NearestObjectivesIfDom.h"
#include "utils.h"
#include <float.h>

void NearestObjectivesIfDom::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	for (int i = 0; i < migrated.size(); i++){
		float minDist = FLT_MAX;
		int indexMin = -1;
		//Search the nearest individual in the population
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
		//Check whether such individual is dominated by the migrated one
		if (dominanceTest(population[indexMin], migrated[i]) == SECOND_DOMINATES){
			delete (population)[indexMin];
			(population)[indexMin] = migrated[i];
		} else {
			delete migrated[i];
		}
	}
}
