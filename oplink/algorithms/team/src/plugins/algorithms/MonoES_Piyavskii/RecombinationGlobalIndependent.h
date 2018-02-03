/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 10/01/13
* Descripcion: clase que implementa la recombinación global 
* independiente, es decir, que los padres para sigma_i y la variable i
* pueden ser diferentes
***********************************************************************/


#ifndef RECOMBINATION_GLOBAL_H
#define RECOMBINATION_GLOBAL_H

#include <vector>
#include <iostream>
#include <stdlib.h>

#include "Recombination.h"
#include "Definitions.h"
#include "utils.h"

using namespace std;

class RecombinationGlobalIndependent : public Recombination {
	public:
		void recombine(vector<Individual *> *, const int mu, const int typeForObjectVariable, const int typeForStrategyVariable, Individual *result);
};

void recombine(Individual **, const int mu, const int typeForObjectVariable, const int typeForStrategyVariable, Individual *result){
	//Debug
	if ((typeForObjectVariable != DISCRETE) && (typeForObjectVariable != INTERMEDIARY)){
		cerr << "Error interno: no se reconoce el codigo de typeForObjectVariable" << endl;
		exit(-1);
	} 
	if ((typeForStrategyVariable != DISCRETE) && (typeForStrategyVariable != INTERMEDIARY)){
		cerr << "Error interno: no se reconoce el codigo de typeForStrategyVariable" << endl;
		exit(-1);
	} 

	//One Step size
	bool recombineStepsi;
	recombineStepsi = (result->objectVariables.size() == result->strategyParameters.size());

	//Strategy if only one sigma
	if (!recombineStepsi){//Solo 1 sigma
		int parent1 = getRandomInteger0_N(mu - 1);
		int parent2 = getRandomInteger0_N(mu - 1);
		if (typeForStrategyVariable == DISCRETE){
			result->strategyParameters[0] = (*population)[((getRandomInteger0_N(1) == 0)?parent1:parent2)]->strategyParameters[i];
		} else if (typeForStrategyVariable == INTERMEDIARY){
			result->strategyParameters[0] = ((*population)[parent1]->strategyParameters[0] + (*population)[parent2]->strategyParameters[0]) / 2.0;
		}
	}

	//Object + strategy (if n sigma)
	for (unsigned int i = 0; i < result->objectVariables.size(); i++){
		int parent1 = getRandomInteger0_N(mu - 1);
		int parent2 = getRandomInteger0_N(mu - 1);
		int selectedParent = ((getRandomInteger0_N(1) == 0)?parent1:parent2);
		//Object
		if (typeForObjectVariable == DISCRETE){
			result->objectVariables[i] = (*population)[selectedParent]->objectVariables[i];
		} else if (typeForObjectVariable == INTERMEDIARY){
			result->objectVariables[i] = ((*population)[parent1]->objectVariables[i] + (*population)[parent2]->objectVariables[i]) / 2.0;
		}

		//Strategy
		if (recombineStepsi){
			int parent3 = getRandomInteger0_N(mu - 1);
			int parent4 = getRandomInteger0_N(mu - 1);
			int selectedParent = ((getRandomInteger0_N(1) == 0)?parent3:parent4);
			if (typeForStrategyVariable == DISCRETE){
				result->strategyVariables[i] = (*population)[selectedParent]->strategyVariables[i];
			} else if (typeForStrategyVariable == INTERMEDIARY){
				result->strategyVariables[i] = ((*population)[parent3]->strategyVariables[i] + (*population)[parent4]->strategyVariables[i]) / 2.0;
			}
		}
	} 
}

#endif
