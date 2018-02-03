#include "MultiObjBestDistThreshold_SelfAdaptive.h"

#include <stdlib.h>
#include <math.h>

void MultiObjBestDistThreshold_SelfAdaptive::convertToMultiObjective(vector<Individual *> *population, vector<Individual *> *offSpring, vector<Individual *> *reference,  int ordObj, int numberGeneration){
	//Look for best individual (in obj0)
	Individual *best = NULL;
	if (population->size() > 0){
		best = (*population)[0];
	} else if (offSpring->size() > 0){
		best = (*offSpring)[0];
	} else {
		cerr << "Error interno: offSpring y population de tam. 0" << endl;
		exit(-1);
	}

	int direction = best->getInternalOptDirection(0);
  int numVar = best->getNumberOfVar();
	double thrSum = 0;
	for (int i = 0; i < population->size(); i++){
    if (thrLocation == MEANPOP_THRVALUE) {
      thrSum += (*population)[i]->getVar(numVar - 1);
    }
		if ( ( (direction == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0)) ) ||
				 ( (direction == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0))) ){
			best = (*population)[i];
		}
	}

	if (population != offSpring){
		for (int i = 0; i < offSpring->size(); i++){
      if (thrLocation == MEANPOP_THRVALUE) {
        thrSum += (*offSpring)[i]->getVar(numVar - 1);
      }
			if ( ( (direction == MINIMIZE) && ((*offSpring)[i]->getObj(0) < best->getObj(0)) ) ||
					 ( (direction == MAXIMIZE) && ((*offSpring)[i]->getObj(0) > best->getObj(0))) ){
				best = (*offSpring)[i];
			}
		}
	}

	double pThreshold;

  // Extracts the threshold value from the best individual
  if (thrLocation == BESTPOP_THRVALUE) {
    pThreshold = best->getVar(best->getNumberOfVar() - 1);
  }
	// Extracts the threshold as the mean of all the thresholds in the population
  else if (thrLocation == MEANPOP_THRVALUE) {
		if (population != offSpring) {
			pThreshold = thrSum / (population->size() + offSpring->size());
		}
		else {
			pThreshold = thrSum / population->size();
		}
  }

	//Calculate distances to the best
	for (int i = 0; i < population->size(); i++){

		if (thrLocation == CORRESP_THRVALUE) {
			pThreshold = (*population)[i]->getVar((*population)[i]->getNumberOfVar() - 1);
		}

		double dist = 0;
		if ( ((best->getInternalOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) - shiftValue >= (best->getObj(0) - shiftValue) * pThreshold)) ||
				 ((best->getInternalOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) - shiftValue <= (best->getObj(0) - shiftValue) / pThreshold)) ){
			for (int j = 0; j < best->getNumberOfVar() - 1; j++){
				dist += ((best->getVar(j) - (*population)[i]->getVar(j)) * (best->getVar(j) - (*population)[i]->getVar(j)));
			}
			(*population)[i]->setObj(ordObj, sqrt(dist));
		} else {
			(*population)[i]->setObj(ordObj, 0);
		}
	}
	if (population != offSpring){
		for (int i = 0; i < offSpring->size(); i++){
		
			if (thrLocation == CORRESP_THRVALUE) {
				pThreshold = (*offSpring)[i]->getVar((*offSpring)[i]->getNumberOfVar() - 1);
			}

			double dist = 0;
			if ( ((best->getInternalOptDirection(0) == MAXIMIZE) && ((*offSpring)[i]->getObj(0) - shiftValue >= (best->getObj(0) - shiftValue) * pThreshold)) ||
					 ((best->getInternalOptDirection(0) == MINIMIZE) && ((*offSpring)[i]->getObj(0) - shiftValue <= (best->getObj(0) - shiftValue) / pThreshold)) ){
				for (int j = 0; j < best->getNumberOfVar() - 1; j++){
					dist += ((best->getVar(j) - (*offSpring)[i]->getVar(j)) * (best->getVar(j) - (*offSpring)[i]->getVar(j)));
				}
				(*offSpring)[i]->setObj(ordObj, sqrt(dist));
			} else {
				(*offSpring)[i]->setObj(ordObj, 0);
			}
		}
	}
}

bool MultiObjBestDistThreshold_SelfAdaptive::init(const vector<string> &params) {
  if (params.size() != 2){
    cerr << "Error, BestDistThreshold parameters --> shiftValue thrLocation" << endl;
    cerr << "thrLocation: " << endl;
    cerr << "0: Threshold value of the best individual in the population" << endl;
    cerr << "1: Mean of the threshold values of all individuals in the population" << endl;
    cerr << "2: Threshold value of every individual applied to itself" << endl;
    return false;
  }

  shiftValue = atof(params[0].c_str());
  thrLocation = atoi(params[1].c_str());

  if ((thrLocation != BESTPOP_THRVALUE) && (thrLocation != MEANPOP_THRVALUE) && (thrLocation != CORRESP_THRVALUE)) {
    cerr << "thrLocation must be 0, 1, or 2" << endl;
    return false;
  }
  return true;
}
