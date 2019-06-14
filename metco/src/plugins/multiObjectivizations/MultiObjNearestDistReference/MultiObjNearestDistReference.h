/***********************************************************************************
 * AUTHORS
 *   - Carlos Segura Gonz�lez
 * 
 * *********************************************************************************/


#ifndef MULTIOBJNEARESTDIST_H 
#define MULTIOBJNEARESTDIST_H 

#include "MultiObjectivization.h"

class MultiObjNearestDistReference : public MultiObjectivization {
	void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference, int ordObj, int numberGeneration);
	int getOptDirection() { return MAXIMIZE; }
};

#endif
