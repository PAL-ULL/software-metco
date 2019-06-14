/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González 
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef MULTIOBJTIMESTAMP_H 
#define MULTIOBJTIMESTAMP_H 

#include "MultiObjectivization.h"

class MultiObjTimeStamp : public MultiObjectivization {
	public:
		MultiObjTimeStamp(){
			currentValue = 0;
		}

		void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference,  int ordObj, int numberGeneration);
		int getOptDirection() { return MINIMIZE; }
	private:
		int currentValue;
};

#endif
