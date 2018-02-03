#include "MultiObjNearestDistThreshold.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>

void MultiObjNearestDistThreshold::convertToMultiObjective(vector<Individual *> *population, vector<Individual *> *offSpring, vector<Individual *> *reference,  int ordObj, int numberGeneration) {

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
  for (int i = 0; i < population->size(); i++){
    if ( ( (direction == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0)) ) ||
         ( (direction == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0))) ){
      best = (*population)[i];
    }
  }

	if (population != offSpring){	
  	for (int i = 0; i < offSpring->size(); i++){
   	 if ( ( (direction == MINIMIZE) && ((*offSpring)[i]->getObj(0) < best->getObj(0)) ) ||
   	      ( (direction == MAXIMIZE) && ((*offSpring)[i]->getObj(0) > best->getObj(0))) ){
   	   best = (*offSpring)[i];
   	 }
  	}
	}

  // Updates the population
  for (unsigned int i = 0; i < population->size(); i++) {
    double minDist = DBL_MAX;
		if ((*population)[i]->getObj(0) - shiftValue < 0){
			cerr << "Objective < 0 after shifting" << endl << flush;
			exit(-1);
		}
    // Checks if the condition of the threshold is satisfied
    if ( (pThreshold == 0) ||
         ((best->getInternalOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) - shiftValue >= (best->getObj(0) - shiftValue)*pThreshold)) ||
         ((best->getInternalOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) - shiftValue <= (best->getObj(0) - shiftValue)/pThreshold)) ) {

      // Respect the own population
      for (unsigned int j = 0; j < population->size(); j++) {
        if (i != j) {
          double actualDist = 0;
          for (unsigned int k = 0; k < (*population)[i]->getNumberOfVar(); k++) {
            actualDist += ((*population)[i]->getVar(k) - (*population)[j]->getVar(k)) * ((*population)[i]->getVar(k) - (*population)[j]->getVar(k));
          }
          if (actualDist < minDist)
            minDist = actualDist;
        }
      }

      // Respect the offSpring
			if (population != offSpring){
      	for (unsigned int j = 0; j < offSpring->size(); j++) {
	        double actualDist = 0;
 	       	for (unsigned int k = 0; k < (*population)[i]->getNumberOfVar(); k++) {
 	        	actualDist += ((*population)[i]->getVar(k) - (*offSpring)[j]->getVar(k)) * ((*population)[i]->getVar(k) - (*offSpring)[j]->getVar(k));
 	       	}
       		if (actualDist < minDist)
          	minDist = actualDist;
      	}
			}

      (*population)[i]->setObj(ordObj, sqrt(minDist));

    } else {
      (*population)[i]->setObj(ordObj, 0);
    }
  }

  // Updates the offSpring
	if (population != offSpring){	
  	for (unsigned int i = 0; i < offSpring->size(); i++) {
    	double minDist = DBL_MAX;
	    
 	   // Checks if the condition of the threshold is satisfied
 	   if ( (pThreshold == 0) ||
          ((best->getInternalOptDirection(0) == MAXIMIZE) && ((*offSpring)[i]->getObj(0) - shiftValue >= (best->getObj(0) - shiftValue)*pThreshold)) ||
 	        ((best->getInternalOptDirection(0) == MINIMIZE) && ((*offSpring)[i]->getObj(0) - shiftValue <= (best->getObj(0) - shiftValue)/pThreshold)) ){

	      // Respect the population
 	     for (unsigned int j = 0; j < population->size(); j++) {
 	       double actualDist = 0;
 	       for (unsigned int k = 0; k < (*offSpring)[i]->getNumberOfVar(); k++) {
 	         actualDist += ((*offSpring)[i]->getVar(k) - (*population)[j]->getVar(k)) * ((*offSpring)[i]->getVar(k) - (*population)[j]->getVar(k));
 	       }
 	       if (actualDist < minDist)
 	         minDist = actualDist;
 	     }

 	     // Respect the own offSpring
 	     for (unsigned int j = 0; j < offSpring->size(); j++) {
 	       if (i != j) {
 	         double actualDist = 0;
 	         for (unsigned int k = 0; k < (*offSpring)[i]->getNumberOfVar(); k++) {
 	           actualDist += ((*offSpring)[i]->getVar(k) - (*offSpring)[j]->getVar(k)) * ((*offSpring)[i]->getVar(k) - (*offSpring)[j]->getVar(k));
 	         }
 	         if (actualDist < minDist)
 	           minDist = actualDist;
 	       }
 	     }

  	    (*offSpring)[i]->setObj(ordObj, sqrt(minDist));

  	  } else {
   	   (*offSpring)[i]->setObj(ordObj, 0);
	    }
	  }
	}
}

bool MultiObjNearestDistThreshold::init(const vector<string> &params) {
  if (params.size() != 2){
    cerr << "Error, NearestDistThreshold parameters --> pThreshold shiftValue" << endl;
    return false;
  }
  pThreshold = atof(params[0].c_str());
	shiftValue = atof(params[1].c_str());
  if ((pThreshold < 0) || (pThreshold > 1)){
    cerr << "Error, pThreshold must be in [0, 1]" << endl;
		return false;
  }
  return true;
}
