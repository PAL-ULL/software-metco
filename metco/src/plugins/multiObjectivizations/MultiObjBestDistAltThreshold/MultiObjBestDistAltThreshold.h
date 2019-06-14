/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González 
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef MULTIOBJBESTDISTALTTHRESHOLD_H 
#define MULTIOBJBESTDISTALTTHRESHOLD_H 

#include "MultiObjectivization.h"

class MultiObjBestDistAltThreshold : public MultiObjectivization {
	void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference,  int ordObj, int numberGeneration);
	int getOptDirection() { return MAXIMIZE; }
};

#endif
