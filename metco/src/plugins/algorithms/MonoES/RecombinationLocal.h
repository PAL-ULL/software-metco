/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 10/01/13
* Descripcion: clase que implementa la recombinación local 
***********************************************************************/


#ifndef RECOMBINATION_LOCAL_H_
#define RECOMBINATION_LOCAL_H

#include <vector>
#include <iostream>
#include <stdlib.h>

#include "Recombination.h"
#include "Definitions.h"
#include "utils.h"

using namespace std;

class RecombinationLocal : public Recombination {
	public:
		void recombine(vector<Individual *> *, const int mu, const int typeForObjectVariable, const int typeForStrategyVariable, Individual *result);
};

void RecombinationLocal::recombine(vector<Individual *> *population, const int mu, const int typeForObjectVariable, const int typeForStrategyVariable, Individual *result){
	//Debug
	if ((typeForObjectVariable != DISCRETE_RECOMBINATION) && (typeForObjectVariable != INTERMEDIARY_RECOMBINATION)){
		cerr << "Error interno: no se reconoce el codigo de typeForObjectVariable" << endl;
		exit(-1);
	} 
	if ((typeForStrategyVariable != DISCRETE_RECOMBINATION) && (typeForStrategyVariable != INTERMEDIARY_RECOMBINATION)){
		cerr << "Error interno: no se reconoce el codigo de typeForStrategyVariable" << endl;
		exit(-1);
	} 

	int parent1 = getRandomInteger0_N(mu - 1);
	int parent2 = getRandomInteger0_N(mu - 1);

	for (unsigned int i = 0; i < result->getNumberOfVar(); i++){//Strategy parameters
		int selectedParent = ((getRandomInteger0_N(1) == 0)?parent1:parent2);
		if (typeForObjectVariable == DISCRETE_RECOMBINATION){
			result->setVar(i, (*population)[selectedParent]->getVar(i));
		} else if (typeForObjectVariable == INTERMEDIARY_RECOMBINATION){
			result->setVar(i, ((*population)[parent1]->getVar(i) + (*population)[parent2]->getVar(i)) / 2.0);
		}
	}
	for (unsigned int i = 0; i < result->getAuxDataSize(); i++){//Sigma parameters
		int selectedParent = ((getRandomInteger0_N(1) == 0)?parent1:parent2);
		if (typeForStrategyVariable == DISCRETE_RECOMBINATION){
			result->setAuxData(i, (*population)[selectedParent]->getAuxData(i));
		} else if (typeForStrategyVariable == INTERMEDIARY_RECOMBINATION){
			result->setAuxData(i, ((*population)[parent1]->getAuxData(i) + (*population)[parent2]->getAuxData(i)) / 2.0);
		}
	} 
}


#endif
