/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González 
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef MULTIOBJNEARESTDISTTHR_H 
#define MULTIOBJNEARESTDISTTHR_H 

#include "MultiObjectivization.h"

class MultiObjNearestDistThresholdReference : public MultiObjectivization {
  public:
	  void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference,  int ordObj, int numberGeneration);
		void convertToMultiObjectiveWithRef(vector<Individual *> *population, vector<Individual *> *reference, int ordObj);
	  int getOptDirection() { return MAXIMIZE; }
    bool init(const vector<string> &params);
  private:
    double pThreshold;
    double shiftValue;
};

#endif
