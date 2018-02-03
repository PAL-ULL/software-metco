/***********************************************************************************
 * AUTORES
 *   - Antonio J. Nebro
 * 
 * FECHA
 *    Febrero 2008
 *
 * DESCRIPCION
 *    Individuo para la resolucion del problema Kursawe, definido en:
 *    F. Kursawe, A variant of evolution strategies for vector optimization, 
 *    in Parallel Problem Solving for Nature, H. Schwefel and R. M¬anner, 
 *    Eds. Berlin, Germany: Springer-Verlag, 1990, pp. 193-197. *    Consiste en: (POR DEFINIR)
 *
 *     Se han definido los metodos requeridos por Individual: init, mutation,
 *     crossover, evaluate y clone
 * ********************************************************************************/

#ifndef __SCHAFFER_H__
#define __SCHAFFER_H__

#include "Individual.h"
#include <math.h>

class Schaffer : public Individual{
public:
	// Initialization method
	bool init (const vector<string> &params);


	// Evaluation
	void evaluate (void);

	// Ranges
  double inline getMaximum(const int i) const { return MAXIMUM[i]; }
  double inline getMinimum(const int i) const { return MINIMUM[i]; }
  unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }


	// Clone
	Individual* clone (void) const;

private:
	// Constants
	static const int NPARAM = 1; 
	static const int NOBJ   = 2;
	static const double MINIMUM[];
	static const double MAXIMUM[];
	static const int OBJFUNC[];	
};

#endif
