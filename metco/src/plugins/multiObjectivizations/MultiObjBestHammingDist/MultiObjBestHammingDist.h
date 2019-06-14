/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González 
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef MULTIOBJBESTHAMMINGDIST_H 
#define MULTIOBJBESTHAMMINGDIST_H 

#include "MultiObjectivization.h"

class MultiObjBestHammingDist : public MultiObjectivization {
	void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference,  int ordObj, int numberGeneration);
	int getOptDirection() { return MAXIMIZE; }
};

#endif
