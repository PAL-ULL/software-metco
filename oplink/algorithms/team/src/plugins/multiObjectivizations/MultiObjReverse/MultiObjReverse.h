/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González 
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef MULTIOBJREVERSE_H 
#define MULTIOBJREVERSE_H 

#include "MultiObjectivization.h"

class MultiObjReverse : public MultiObjectivization {
	public:
		void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference,  int ordObj, int numberGeneration);
		void initiallization(Individual *ind) {
			if (ind->getInternalOptDirection(0) == MINIMIZE){
				direction = MAXIMIZE;
			} else {
				direction = MINIMIZE;
			}
		}
		int getOptDirection() { return direction; }
	private:
		int direction;
};

#endif
