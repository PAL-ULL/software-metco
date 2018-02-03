/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González 
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef MULTIOBJBESTDISTTHRESHOLD_H 
#define MULTIOBJBESTDISTTHRESHOLD_H 

#include "MultiObjectivization.h"

class MultiObjBestDistThreshold : public MultiObjectivization {
  public:
		void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference,  int ordObj, int numberGeneration);
		int getOptDirection() { return MAXIMIZE; }
  	bool init(const vector<string> &params);
  private:
    double pThreshold;
    double shiftValue;
};

#endif
