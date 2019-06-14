#include "MultiObjBestNormDistAltThreshold.h"

#include <stdlib.h>
#include <math.h>

#define THRESHOLD 0.75//TODO: esto deberia ser un parametro del plugin
void MultiObjBestNormDistAltThreshold::convertToMultiObjective(vector<Individual *> *population, vector<Individual *> *offSpring, vector<Individual *> *reference,  int ordObj, int numberGeneration){
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
	for (int i = 0; i < population->size(); i++){
		if ( ( (direction == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0)) ) ||
				 ( (direction == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0))) ){
			best = (*population)[i];
		}
	}

	for (int i = 0; i < offSpring->size(); i++){
		if ( ( (direction == MINIMIZE) && ((*offSpring)[i]->getObj(0) < best->getObj(0)) ) ||
				 ( (direction == MAXIMIZE) && ((*offSpring)[i]->getObj(0) > best->getObj(0))) ){
			best = (*offSpring)[i];
		}
	}

	//Calculate distances to the best
	double maxDistance = 0;

	for (int i = 0; i < population->size(); i++){
		double dist = 0;
		for (int j = 0; j < best->getNumberOfVar(); j++){
			dist += (best->getVar(j) - (*population)[i]->getVar(j)) * (best->getVar(j) - (*population)[i]->getVar(j));
		}
		double distance = sqrt(dist);
		(*population)[i]->setObj(ordObj, distance);
		if (distance > maxDistance){
			maxDistance = distance;
		}
	}
	for (int i = 0; i < offSpring->size(); i++){
		double dist = 0;
		for (int j = 0; j < best->getNumberOfVar(); j++){
			dist += (best->getVar(j) - (*offSpring)[i]->getVar(j)) * (best->getVar(j) - (*offSpring)[i]->getVar(j));
		}
		double distance = sqrt(dist);
		(*offSpring)[i]->setObj(ordObj, distance);
		if (distance > maxDistance){
			maxDistance = distance;
		}
	}

	if (maxDistance != 0){
		for (int i = 0; i < population->size(); i++){
			double normalized = ((*population)[i]->getObj(ordObj) / maxDistance);
			if (normalized <= THRESHOLD){
				(*population)[i]->setObj(ordObj, normalized);
			} else {
				(*population)[i]->setObj(ordObj, 0);
			}
		}
		for (int i = 0; i < offSpring->size(); i++){
			double normalized = ((*offSpring)[i]->getObj(ordObj) / maxDistance);
			if (normalized <= THRESHOLD){
				(*offSpring)[i]->setObj(ordObj, normalized);
			} else {
				(*offSpring)[i]->setObj(ordObj, 0);
			}
		}
	}
}
