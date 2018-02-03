/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 07/01/13
* Descripcion: clase que representa la mutación con un step size 
* Recibe los parámetros:
*   - Tau
*   - Epsilon
*   - StrategyParametersMaximumValues 
*
* Si una variable se pasa de su límite se resetea justo al límite
***********************************************************************/

#ifndef _MUTATIONONESTEPSIZE_H_
#define _MUTATIONONESTEPSIZE_H_

#include <iostream>
#include <math.h>

#include "MutationES.h"
#include "utils.h"

class MutationOneStepSize : public MutationES {
	public: 
		void initialize(const vector<double> &parameters);
		void mutate(Individual *);
	private:
		double tau;
		double epsilon;
		double maxSigma;
};

using namespace std;

void MutationOneStepSize::initialize(const vector<double> &parameters){
	tau = parameters[0];
	epsilon = parameters[1];
	maxSigma = parameters[2];
}

void MutationOneStepSize::mutate(Individual *ind){
	//Mutate sigma value
	ind->setAuxData(0, ind->getAuxData(0) * exp(tau * generateRandomNormal(0, 1)));
	ind->setAuxData(0, max(ind->getAuxData(0), epsilon));
	ind->setAuxData(0, min(ind->getAuxData(0), maxSigma));
	//Mutate variables
	for (int i = 0; i < ind->getNumberOfVar(); i++){
		ind->setVar(i, ind->getVar(i) + ind->getAuxData(0) * generateRandomNormal(0, 1));
		ind->setVar(i, max(ind->getVar(i), ind->getMinimum(i)));
		ind->setVar(i, min(ind->getVar(i), ind->getMaximum(i)));
	}
}

#endif
