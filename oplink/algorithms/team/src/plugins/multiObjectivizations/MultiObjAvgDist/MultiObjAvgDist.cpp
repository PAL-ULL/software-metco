#include "MultiObjAvgDist.h"

#include <stdlib.h>
#include <math.h>

void MultiObjAvgDist::convertToMultiObjective(vector<Individual *> *population, vector<Individual *> *offSpring, vector<Individual *> *reference,  int ordObj, int numberGeneration) {

  // Updates the population
  for (unsigned int i = 0; i < population->size(); i++) {
    double totalDist = 0;
    // Respect the own population
    for (unsigned int j = 0; j < population->size(); j++) {
      double actualDist = 0;
      if (i != j) {
        for (unsigned int k = 0; k < (*population)[i]->getNumberOfVar(); k++) {
          actualDist += ((*population)[i]->getVar(k) - (*population)[j]->getVar(k)) * ((*population)[i]->getVar(k) - (*population)[j]->getVar(k));
        }
        totalDist += sqrt(actualDist);
      }
    }
    // Respect the offSpring
		if (population != offSpring){
	    for (unsigned int j = 0; j < offSpring->size(); j++) {
 	     double actualDist = 0;
 	     for (unsigned int k = 0; k < (*population)[i]->getNumberOfVar(); k++) {
 	       actualDist += ((*population)[i]->getVar(k) - (*offSpring)[j]->getVar(k)) * ((*population)[i]->getVar(k) - (*offSpring)[j]->getVar(k));
 	     }
 	     totalDist += sqrt(actualDist);
 	   }
 	   (*population)[i]->setObj(ordObj, totalDist / (population->size() + offSpring->size() - 1));
		}
  }

  // Updates the offSpring
	if (population != offSpring){
	  for (unsigned int i = 0; i < offSpring->size(); i++) {
 	  	double totalDist = 0;
    	// Respect the population
	    for (unsigned int j = 0; j < population->size(); j++) {
 	     double actualDist = 0;
 	     for (unsigned int k = 0; k < (*offSpring)[i]->getNumberOfVar(); k++) {
 	       actualDist += ((*offSpring)[i]->getVar(k) - (*population)[j]->getVar(k)) * ((*offSpring)[i]->getVar(k) - (*population)[j]->getVar(k));
 	     }
 	     totalDist += sqrt(actualDist);
 	   }
 	   // Respect the own offSpring
 	   for (unsigned int j = 0; j < offSpring->size(); j++) {
 	     double actualDist = 0;
 	     if (i != j) {
 	       for (unsigned int k = 0; k < (*offSpring)[i]->getNumberOfVar(); k++) {
 	         actualDist += ((*offSpring)[i]->getVar(k) - (*offSpring)[j]->getVar(k)) * ((*offSpring)[i]->getVar(k) - (*offSpring)[j]->getVar(k));
 	       }
 	       totalDist += sqrt(actualDist);
 	     }
 	   }
 	   (*offSpring)[i]->setObj(ordObj, totalDist / (population->size() + offSpring->size() - 1));
		}
	}
}
