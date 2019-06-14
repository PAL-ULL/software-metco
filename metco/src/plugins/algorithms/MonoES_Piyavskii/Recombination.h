/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 07/01/13
* Descripcion: clase base que debemos heredar para implementar un
* nuevo tipo de recombinación.
* Nota: en las descripciones de los algoritmos no queda claro si en el 
* caso de que hayan n step sizes, se debe elegir los mismos padres para 
* la variable i y el sigma_i (en algunos parece una cosa,
* y en otros la contraria)
* Por ejemplo, en el artículo de Beyer parece que son independientes, 
* pues primero se llama a s_recombination y luego a y_recombination con
* lo que se tratan como genes independientes.
* Por tanto, se han implementado las 2 opciones y se realiza una 
* comparativa.
* Nota: aunque se seleccionen seleccionen los mismos padres, sí que
* se generan 2 numeros aleatorios, con lo que en la recombinación
* discreta se podría heredar el s de uno y el y de otro.
* En caso de usar recombinación local no hay diferencia.
***********************************************************************/


#ifndef RECOMBINATION_H
#define RECOMBINATION_H

#include <vector>

#include "Individual.h"

using namespace std;

//Codes for the type of recombination
#define DISCRETE_RECOMBINATION 10
#define INTERMEDIARY_RECOMBINATION 11

class Recombination {
	public:
		virtual ~Recombination(){}
		virtual void recombine(vector<Individual*> *, const int mu, const int typeForObjectVariable, const int typeForStrategyVariable, Individual *result) = 0;
};

#endif
