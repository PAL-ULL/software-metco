/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 10/01/13
* Descripcion: clase que implementa la recombinación global 
***********************************************************************/


#ifndef RECOMBINATION_GLOBAL_H
#define RECOMBINATION_GLOBAL_H

#include <vector>
#include <stdlib.h>
#include <iostream>

#include "Recombination.h"
#include "Definitions.h"
#include "utils.h"

using namespace std;

class RecombinationGlobal : public Recombination {
	public:
		void recombine(vector<Individual *> *, const int mu, const int typeForObjectVariable, const int typeForStrategyVariable, Individual *result);
};

void RecombinationGlobal::recombine(vector<Individual *> *population, const int mu, const int typeForObjectVariable, const int typeForStrategyVariable, Individual *result){
	//Debug
	if ((typeForObjectVariable != DISCRETE_RECOMBINATION) && (typeForObjectVariable != INTERMEDIARY_RECOMBINATION)){
		cerr << "Error interno: no se reconoce el codigo de typeForObjectVariable" << endl;
		exit(-1);
	} 
	if ((typeForStrategyVariable != DISCRETE_RECOMBINATION) && (typeForStrategyVariable != INTERMEDIARY_RECOMBINATION)){
		cerr << "Error interno: no se reconoce el codigo de typeForStrategyVariable" << endl;
		exit(-1);
	} 

	//One Step size
	bool recombineStepsi;
	recombineStepsi = (result->getNumberOfVar() == result->getAuxDataSize());

	//Strategy if only one sigma
	if (!recombineStepsi){//Solo 1 sigma
		int parent1 = getRandomInteger0_N(mu - 1);
		int parent2 = getRandomInteger0_N(mu - 1);
		if (typeForStrategyVariable == DISCRETE_RECOMBINATION){
			result->setAuxData(0, (*population)[((getRandomInteger0_N(1) == 0)?parent1:parent2)]->getAuxData(0));
		} else if (typeForStrategyVariable == INTERMEDIARY_RECOMBINATION){
			result->setAuxData(0, ((*population)[parent1]->getAuxData(0) + (*population)[parent2]->getAuxData(0)) / 2.0);
		}
	}

	//Object + strategy (if n sigma)
	for (unsigned int i = 0; i < result->getNumberOfVar(); i++){
		int parent1 = getRandomInteger0_N(mu - 1);
		int parent2 = getRandomInteger0_N(mu - 1);
		int selectedParent = ((getRandomInteger0_N(1) == 0)?parent1:parent2);
		//Object
		if (typeForObjectVariable == DISCRETE_RECOMBINATION){
			result->setVar(i, (*population)[selectedParent]->getVar(i));
		} else if (typeForObjectVariable == INTERMEDIARY_RECOMBINATION){
			result->setVar(i, ((*population)[parent1]->getVar(i) + (*population)[parent2]->getVar(i)) / 2.0);
		}
		//Strategy
		if (recombineStepsi){
			if (typeForStrategyVariable == DISCRETE_RECOMBINATION){
				result->setAuxData(i, (*population)[selectedParent]->getAuxData(i));
			} else if (typeForStrategyVariable == INTERMEDIARY_RECOMBINATION){
				result->setAuxData(i, ((*population)[parent1]->getAuxData(i) + (*population)[parent2]->getAuxData(i)) / 2.0);
			}
		}
	} 
}

#endif
