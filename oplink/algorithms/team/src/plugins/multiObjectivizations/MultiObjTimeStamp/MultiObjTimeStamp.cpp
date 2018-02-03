#include "MultiObjTimeStamp.h"

#include <stdlib.h>

void MultiObjTimeStamp::convertToMultiObjective(vector<Individual *> *population, vector<Individual *> *offSpring, vector<Individual *> *reference,  int ordObj, int numberGeneration){
	cerr << "OJO: no valido en paralelo con migraciones!!!" << endl;
	if (currentValue == 0){
		for (int i = 0; i < population->size(); i++){
			(*population)[i]->setObj(ordObj, currentValue++);
		}
	} else {
		for (int i = 0; i < offSpring->size(); i++){
			(*offSpring)[i]->setObj(ordObj, currentValue);
		}
		currentValue++;
	}
}
