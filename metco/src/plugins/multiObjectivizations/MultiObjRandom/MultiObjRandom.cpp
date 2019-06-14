#include "MultiObjRandom.h"

#include <stdlib.h>

void MultiObjRandom::convertToMultiObjective(vector<Individual *> *population, vector<Individual *> *offSpring, vector<Individual *> *reference,  int ordObj, int numberGeneration){
	for (int i = 0; i < offSpring->size(); i++){
		(*offSpring)[i]->setObj(ordObj, (double)(rand()));		
	}
}
