/************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    November 2008
 ************************************************************************************/

#include "NearestVariablesIfDom.h"
#include "utils.h"

#include <float.h>

void NearestVariablesIfDom::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	for (int i = 0; i < migrated.size(); i++){
		float minDist = FLT_MAX;
		int indexMin = -1;
		//Search the nearest individual
		for (int j = 0; j < population.size(); j++){
			float dist = 0;
			for (int k = 0; k < migrated[i]->getNumberOfVar(); k++){
				dist += ( (migrated[i]->getVar(k) - population[j]->getVar(k)) * (migrated[i]->getVar(k) - population[j]->getVar(k)) );
			}
			if (dist < minDist){
				minDist = dist;
				indexMin = j;
			}
		}
		//Check whether is dominated
		if (dominanceTest(population[indexMin], migrated[i]) == SECOND_DOMINATES){
			delete (population)[indexMin];
			(population)[indexMin] = migrated[i];
		} else {
			delete migrated[i];
		}
	}
}
