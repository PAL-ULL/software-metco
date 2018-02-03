/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González 
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef MULTIOBJAVGDISTTHR_H
#define MULTIOBJAVGDISTTHR_H

#include "MultiObjectivization.h"

class MultiObjAvgDistThreshold : public MultiObjectivization {
	public:
		void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference, int ordObj, int numberGeneration);
		int getOptDirection() { return MAXIMIZE; }
    bool init(const vector<string> &params);
  private:
		double pThreshold;
    double shiftValue;
};

#endif
