/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo Gonz�lez 
 *   - Carlos Segura Gonz�lez
 * 
 * *********************************************************************************/


#ifndef MULTIOBJAVGDIST_H
#define MULTIOBJAVGDIST_H

#include "MultiObjectivization.h"

class MultiObjAvgDist : public MultiObjectivization {
	void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference, int ordObj, int numberGeneration);
	int getOptDirection() { return MAXIMIZE; }
};

#endif
