/************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *    Eduardo Segredo Gonzalez
 *
 * DATE
 *    July 2011
 ***********************************************************************************/

#include "NearestVariablesHammingIfBestMono.h"
#include "utils.h"
#include <limits.h>

void NearestVariablesHammingIfBestMono::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	for (int i = 0; i < migrated.size(); i++){
	  bool foundBetter = false;
    for (int j = 0; j < population.size(); j++) {
      if (((population[j]->getObj(0) > migrated[i]->getObj(0)) && (population[j]->getInternalOptDirection(0) == MAXIMIZE)) || 
	       ((population[j]->getObj(0) < migrated[i]->getObj(0)) && (population[j]->getInternalOptDirection(0) == MINIMIZE))) {
			  foundBetter = true;
				break;
			}
		}

		if (!foundBetter) {
			int minDist = INT_MAX;
			int indexMin = 0;
			//Search the nearest individual
			for (int j = 0; j < population.size(); j++){
				int dist = 0;
				for (int k = 0; k < migrated[i]->getNumberOfVar(); k++){
					if (migrated[i]->getVar(k) != population[j]->getVar(k)){
						dist++;
					}
				}
				if (dist < minDist){
					minDist = dist;
					indexMin = j;
				}
			}
  	  delete (population)[indexMin];
  	  (population)[indexMin] = migrated[i];
	  } else {
  	  delete migrated[i];
		}
	}
}
