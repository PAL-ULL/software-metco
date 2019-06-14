/***********************************************************************************
 * AUTORES
 *   - Antonio J. Nebro
 * 
 * FECHA
 *    Febrero 2008
 *
 * DESCRIPCION
 *    Individuo para la resolucion del problema Fonseca
 *
 *     Se han definido los metodos requeridos por Individual: init, mutation,
 *     crossover, evaluate y clone
 * ********************************************************************************/

#ifndef __Fonseca_H__
#define __Fonseca_H__

#include "Individual.h"
#include <math.h>

class Fonseca : public Individual{
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
	// Constants
	static const int NPARAM = 3; 
	static const int NOBJ   = 2;
	static const double MINIMUM[];
	static const double MAXIMUM[];
	static const int OBJFUNC[];	
};

#endif
