/***********************************************************************************
 * AUTORES
 *   - Carlos Segura González
 * 
 * FECHA
 * 	 Mayo de 2010
 *
 * DESCRIPCION
 * 	Problema Leading Zeros Trailing Ones. Es un problema combinatorio biobjetivo
 * 	utilizado para propositos de test.
 * 	El individuo se representa por un string binario de tam. N (parametro del 
 * 	problema). En realidad pueden ser double [0, 1], y se considerara [0, 0.5] --> 0
 * 	(0.5, 1] --> 1. 
 * 	Si usamos los operadores tipicos de binarios no influye en nada.
 * 	Obj 1: numero de ceros consecutivos que estan al final del string binario
 * 	Obj 2: numero de unos consecutivos que estan al principio del string binario
 *
 * 	Nota: la evaluacion la hacemos siempre completa, se podria integrar con las
 * 	mutaciones de forma facil, pero de todas formas no es una evaluacion cara,
 * 	asi que prefiero dejarlo mas generico
 * ********************************************************************************/

#ifndef __LZTO_H__
#define __LZTO_H__

#include "Individual.h"

class LZTO : public Individual {
public:
	// Initialization method
	bool init (const vector<string> &params);
	void restart();

	// Evaluation
	void evaluate (void);
	

	// Ranges
	double inline getMaximum(const int i) const { return 1; }
	double inline getMinimum(const int i) const { return 0; }
	unsigned int inline getOptDirection(const int i) const { return MAXIMIZE; }

	// Clone
	Individual* clone (void) const;

	//Local Search
	unsigned int getNeighbourhoodSize() const; 
	Individual* getNeighbour(const int i) const;
	void getNeighbourObjectives(const int neigh, vector<double> &objectives) const ;


private:
	// Constants
	static const int NOBJ   = 2;
};

#endif
