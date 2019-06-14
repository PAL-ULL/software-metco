/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González 
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef MULTIOBJNEARESTDISTTHR_SELFADAPTIVE_H 
#define MULTIOBJNEARESTDISTTHR_SELFADAPTIVE_H 

#include "MultiObjectivization.h"

#define BESTPOP_THRVALUE 0
#define MEANPOP_THRVALUE 1
#define CORRESP_THRVALUE 2

class MultiObjNearestDistThreshold_SelfAdaptive : public MultiObjectivization {
  public:
	  void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference,  int ordObj, int numberGeneration);
	  int getOptDirection() { return MAXIMIZE; }
    bool init(const vector<string> &params);
  private:
    double shiftValue;
		int thrLocation;
};

#endif
