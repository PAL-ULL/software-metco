#include "MultiObjAvgDistThreshold_SelfAdaptive.h"

#include <stdlib.h>
#include <math.h>

void MultiObjAvgDistThreshold_SelfAdaptive::convertToMultiObjective(vector<Individual *> *population, vector<Individual *> *offSpring, vector<Individual *> *reference,  int ordObj, int numberGeneration) {

  //Looks for best individual (in obj0)
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
	// Extracts the threshold value as the mean of all individuals in the population
  else if (thrLocation == MEANPOP_THRVALUE) {
		if (population != offSpring) {
			pThreshold = thrSum / (population->size() + offSpring->size());
		}
		else {
			pThreshold = thrSum / population->size();
		}
  }
	
  // Updates the population
  for (unsigned int i = 0; i < population->size(); i++) {
		
		// Extracts the threshold value from the corresponding individual
		if (thrLocation == CORRESP_THRVALUE) {
			pThreshold = (*population)[i]->getVar((*population)[i]->getNumberOfVar() - 1);
		}

    // Checks if the condition of the threshold is satisfied
    if ( (pThreshold == 0) ||
         ((best->getInternalOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) - shiftValue >= (best->getObj(0) - shiftValue)*pThreshold)) ||
         ((best->getInternalOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) - shiftValue <= (best->getObj(0) - shiftValue)/pThreshold)) ) {

	    double totalDist = 0;
			// Respect the own population
	    for (unsigned int j = 0; j < population->size(); j++) {
				double actualDist = 0;
				if (i != j) {
					for (unsigned int k = 0; k < (*population)[i]->getNumberOfVar() - 1; k++) {
          	actualDist += ((*population)[i]->getVar(k) - (*population)[j]->getVar(k)) * ((*population)[i]->getVar(k) - (*population)[j]->getVar(k));
        	}
        	totalDist += sqrt(actualDist);
      	}
    	}
	    // Respect the offSpring
			if (population != offSpring){
		    for (unsigned int j = 0; j < offSpring->size(); j++) {
					double actualDist = 0;
					for (unsigned int k = 0; k < (*population)[i]->getNumberOfVar() - 1; k++) {
 	      	 	actualDist += ((*population)[i]->getVar(k) - (*offSpring)[j]->getVar(k)) * ((*population)[i]->getVar(k) - (*offSpring)[j]->getVar(k));
 	     		}
 	     		totalDist += sqrt(actualDist);
 	   		}
			}
			if (population != offSpring) {
 	   		(*population)[i]->setObj(ordObj, totalDist / (population->size() + offSpring->size() - 1));
			} else {
 	   		(*population)[i]->setObj(ordObj, totalDist / (population->size() - 1));
			}
		} else {
			(*population)[i]->setObj(ordObj, 0);
		}
	}

  // Updates the offSpring
	if (population != offSpring){
	  for (unsigned int i = 0; i < offSpring->size(); i++) {
		
			// Extracts the threshold value from the corresponding individual
			if (thrLocation == CORRESP_THRVALUE) {
				pThreshold = (*offSpring)[i]->getVar((*offSpring)[i]->getNumberOfVar() - 1);
			}

			// Checks if the condition of the threshold is satisfied
			if ( (pThreshold == 0) ||
          ((best->getInternalOptDirection(0) == MAXIMIZE) && ((*offSpring)[i]->getObj(0) - shiftValue >= (best->getObj(0) - shiftValue)*pThreshold)) ||
          ((best->getInternalOptDirection(0) == MINIMIZE) && ((*offSpring)[i]->getObj(0) - shiftValue <= (best->getObj(0) - shiftValue)/pThreshold)) ){
	
				double totalDist = 0;
    		// Respect the population
	    	for (unsigned int j = 0; j < population->size(); j++) {
 	     		double actualDist = 0;
 	     		for (unsigned int k = 0; k < (*offSpring)[i]->getNumberOfVar() - 1; k++) {
 	       		actualDist += ((*offSpring)[i]->getVar(k) - (*population)[j]->getVar(k)) * ((*offSpring)[i]->getVar(k) - (*population)[j]->getVar(k));
 	     		}
 	     		totalDist += sqrt(actualDist);
 	   		}
 	   		// Respect the own offSpring
 	   		for (unsigned int j = 0; j < offSpring->size(); j++) {
 	     		double actualDist = 0;
 	     		if (i != j) {
 	       		for (unsigned int k = 0; k < (*offSpring)[i]->getNumberOfVar() - 1; k++) {
 	         		actualDist += ((*offSpring)[i]->getVar(k) - (*offSpring)[j]->getVar(k)) * ((*offSpring)[i]->getVar(k) - (*offSpring)[j]->getVar(k));
 	       		}
 	       		totalDist += sqrt(actualDist);
 	     		}
 	   		}
 	   		(*offSpring)[i]->setObj(ordObj, totalDist / (population->size() + offSpring->size() - 1));
			} else {
				(*offSpring)[i]->setObj(ordObj, 0);
			}
		}
	}
}

bool MultiObjAvgDistThreshold_SelfAdaptive::init(const vector<string> &params) {
  if (params.size() != 2){
    cerr << "Error, AvgDistThreshold parameters --> shiftValue thrLocation" << endl;
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
