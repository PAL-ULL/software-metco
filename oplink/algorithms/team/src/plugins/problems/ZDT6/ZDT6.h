/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 *
 * DESCRIPCION
 *    Individuo para la resolucion del problema de Benchmark ZDT6.
 *    El problema ZDT6 esta definido en:
 *       Comparison of Multiobjective Evolutionary Algorithms: Empirical Results
 *       Zitzler, E. and Deb, K. and Thiele, L.
 * ********************************************************************************/

#ifndef __INDIVIDUALZDT6_H__
#define __INDIVIDUALZDT6_H__

#include "Individual.h"
#include <math.h>

class ZDT6 : public Individual{
public:
	// Initialization method
	bool init (const vector<string> &params);


	// Evaluation
	void evaluate (void);

	// Clone
	Individual* clone (void) const;

	// Ranges
	double inline getMaximum(const int i) const { return MAXIMUM[i]; }
	double inline getMinimum(const int i) const { return MINIMUM[i]; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

private:
	// Genetic Operators
	void dependentCrossover (Individual* ind);	
	void dependentMutation (double pm);
	// Constants
	static const int NPARAM = 10; 
	static const int NOBJ   = 2;
	static const double MINIMUM[];
	static const double MAXIMUM[];
	static const int OBJFUNC[];	
};

#endif
