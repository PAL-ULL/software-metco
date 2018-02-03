/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo Gonz�lez 
 *   - Carlos Segura Gonz�lez
 * 
 * *********************************************************************************/


#ifndef MULTIOBJRANDOM_H 
#define MULTIOBJRANDOM_H 

#include "MultiObjectivization.h"

class MultiObjRandom : public MultiObjectivization {
	void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference,  int ordObj, int numberGeneration);
	int getOptDirection() { return MAXIMIZE; }
};

#endif
