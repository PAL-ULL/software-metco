/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 07/01/13
* Descripcion: clase base que debemos heredar para implementar un
nuevo tipo de mutación
***********************************************************************/


#ifndef MUTATION_ES_H
#define MUTATION_ES_H

#include <vector>

#include "Individual.h"

using namespace std;

class MutationES {
	public:
		virtual ~MutationES(){}
		virtual void initialize(const vector<double> &parameters) = 0;
		virtual void mutate(Individual *) = 0;
};
#endif
