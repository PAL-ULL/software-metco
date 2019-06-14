/********************************************************************************************
 * AUTHOR
 *    Carlos Segura Gonzalez
 *
 * DATE
 *    January 2009
 * *******************************************************************************************/

#include "MonoFirstIfBetter.h"
#include "utils.h"

#include <math.h>

void MonoFirstIfBetter::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	if (migrated.size() != 1){
		cerr << "Warning: migrating more than 1 individual. Only the first is used" << endl;
	}
	if (population.size() < 2){
		cerr << "Warning: population without enough individuals" << endl;
	}
	//Se acepta si es mejor que el best (y sustituye al best y al primer elemento)
	if ( ((population[0]->getInternalOptDirection(0) == MINIMIZE) && (migrated[0]->getObj(0) < population[0]->getObj(0))) ||
		 ((population[0]->getInternalOptDirection(0) == MAXIMIZE) && (migrated[0]->getObj(0) > population[0]->getObj(0))) ){
			delete population[0];
			delete population[1];
			population[0] = migrated[0];
			population[1] = migrated[0]->internalClone();
	} else {
		delete migrated[0];
	}
}
