/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González 
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef MULTIOBJBESTNORMDISTALTTHRESHOLD_H 
#define MULTIOBJBESTNORMDISTALTTHRESHOLD_H 

#include "MultiObjectivization.h"

class MultiObjBestNormDistAltThreshold : public MultiObjectivization {
	void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference,  int ordObj, int numberGeneration);
	int getOptDirection() { return MAXIMIZE; }
};

#endif
