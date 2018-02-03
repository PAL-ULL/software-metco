/************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *    Eduardo Segredo Gonzalez
 *
 * DATE
 *    July 2011
 ***********************************************************************************/

#include "NearestObjectivesIfBestMono.h"
#include "utils.h"
#include <float.h>

void NearestObjectivesIfBestMono::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
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
  	  delete (population)[indexMin];
  	  (population)[indexMin] = migrated[i];
	  } else {
  	  delete migrated[i];
		}
	}
}
