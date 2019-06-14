#include "MultiObjNearestDistReference.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>

//TODO
//Con el NSGA-II cada vez es llamado con un individuo mas, con lo que podria calcularse solo sobre el ultimo
//Pero este es mas general

void MultiObjNearestDistReference::convertToMultiObjective(vector<Individual *> *population, vector<Individual *> *offSpring, vector<Individual *> *reference, int ordObj, int numberGeneration) {
	if ((population == reference) && (offSpring == reference)){//For Parent selection
  	for (unsigned int i = 0; i < population->size(); i++) {
    	double minDist = DBL_MAX;
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
    	(*population)[i]->setObj(ordObj, sqrt(minDist));
  	}
		return;
	}
  // Updates the population
  for (unsigned int i = 0; i < population->size(); i++) {
    double minDist = DBL_MAX;
    // Respect the reference 
    for (unsigned int j = 0; j < reference->size(); j++) {
      double actualDist = 0;
      for (unsigned int k = 0; k < (*population)[i]->getNumberOfVar(); k++) {
        actualDist += ((*population)[i]->getVar(k) - (*reference)[j]->getVar(k)) * ((*population)[i]->getVar(k) - (*reference)[j]->getVar(k));
      }
      if (actualDist < minDist){
        minDist = actualDist;
			}
    }
    (*population)[i]->setObj(ordObj, sqrt(minDist));
  }

  // Updates the offSpring
	if (offSpring != NULL){	
	  for (unsigned int i = 0; i < offSpring->size(); i++) {
 	    double minDist = DBL_MAX;
 	    // Respect the reference
 	    for (unsigned int j = 0; j < reference->size(); j++) {
 	      double actualDist = 0;
 	      for (unsigned int k = 0; k < (*offSpring)[i]->getNumberOfVar(); k++) {
 	        actualDist += ((*offSpring)[i]->getVar(k) - (*reference)[j]->getVar(k)) * ((*offSpring)[i]->getVar(k) - (*reference)[j]->getVar(k));
 	      }
 	      if (actualDist < minDist)
 	        minDist = actualDist;
 	    }
 	    (*offSpring)[i]->setObj(ordObj, sqrt(minDist));
 	  }
	}
}
