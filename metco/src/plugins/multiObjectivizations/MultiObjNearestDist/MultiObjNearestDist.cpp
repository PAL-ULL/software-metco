#include "MultiObjNearestDist.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>

void MultiObjNearestDist::convertToMultiObjective(vector<Individual *> *population, vector<Individual *> *offSpring, vector<Individual *> *reference,  int ordObj, int numberGeneration) {

  // Updates the population
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
  }

  // Updates the offSpring
	if (population != offSpring){	
	  for (unsigned int i = 0; i < offSpring->size(); i++) {
 	   double minDist = DBL_MAX;
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
 	 }
	}
}
