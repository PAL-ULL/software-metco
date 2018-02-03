/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González 
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef MULTIOBJBESTDIST_H 
#define MULTIOBJBESTDIST_H 

#include "MultiObjectivization.h"

class MultiObjBestDist : public MultiObjectivization {
	void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference,  int ordObj, int numberGeneration);
	int getOptDirection() { return MAXIMIZE; }
};

#endif
