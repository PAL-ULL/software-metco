/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 07/01/13
* Descripcion: clase que representa la mutación con N step size 
* Recibe los parámetros:
*   - Tau
*   - Epsilon
*   - TauPrim
*   - N StrategyParametersMaximumValues 
*
* Si una variable se pasa de su límite se resetea justo al límite
***********************************************************************/

#ifndef _MUTATIONNSTEPSIZE_H_
#define _MUTATIONNSTEPSIZE_H_

#include <iostream>
#include <math.h>

#include "MutationES.h"
#include "utils.h"

class MutationNStepSize : public MutationES {
	public: 
		void initialize(const vector<double> &parameters);
		void mutate(Individual *);
	private:
		double tau;
		double epsilon;
		double tauprim;
		vector<double> maxSigma;
};

using namespace std;

void MutationNStepSize::initialize(const vector<double> &parameters){
	tau = parameters[0];
	epsilon = parameters[1];
	tauprim = parameters[2];
	for (unsigned int i = 3; i < parameters.size(); i++){
		maxSigma.push_back(parameters[i]);
	}
}

void MutationNStepSize::mutate(Individual *ind){
	//Mutate lambda values
	double fixedRandom = generateRandomNormal(0, 1);
	for (int i = 0; i < ind->getNumberOfVar(); i++){
		ind->setAuxData(i, ind->getAuxData(i) * exp(tauprim * fixedRandom + tau * generateRandomNormal(0, 1)));
		ind->setAuxData(i, max(ind->getAuxData(i), epsilon));
		ind->setAuxData(i, min(ind->getAuxData(i), maxSigma[i]));
	}
	//Mutate variables
	for (int i = 0; i < ind->getNumberOfVar(); i++){
		ind->setVar(i, ind->getVar(i) + ind->getAuxData(i) * generateRandomNormal(0, 1));
		ind->setVar(i, max(ind->getVar(i), ind->getMinimum(i)));
		ind->setVar(i, min(ind->getVar(i), ind->getMaximum(i)));
	}
}

#endif
