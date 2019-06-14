/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo Gonz�lez 
 *   - Carlos Segura Gonz�lez
 * 
 * *********************************************************************************/


#ifndef MULTIOBJBESTHAMMINGDISTALTTHRESHOLD_H 
#define MULTIOBJBESTHAMMINGDISTALTTHRESHOLD_H 

#include "MultiObjectivization.h"

class MultiObjBestHammingDistAltThreshold : public MultiObjectivization {
	void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference,  int ordObj, int numberGeneration);
	int getOptDirection() { return MAXIMIZE; }
};

#endif
