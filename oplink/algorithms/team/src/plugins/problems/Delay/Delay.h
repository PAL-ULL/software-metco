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
 *    Individuo para la resolucion del problema de Benchmark ZDT1 al que se le añade
 *    un delay en la evaluacion.
 * ********************************************************************************/

#ifndef __Delay_H__
#define __Delay_H__

#include "Individual.h"
#include <math.h>

class Delay : public Individual {
public:
	// Initialization method
	bool init (const vector<string> &params);


	// Evaluation
	void evaluate (void);

	// Ranges
	double inline getMaximum(const int i) const { return 1; }
	double inline getMinimum(const int i) const { return 0; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

	// Clone
	Individual* clone (void) const;

private:
	// Genetic Operators
	void dependentCrossover (Individual* ind);	
	void dependentMutation (double pm);
	// Constants
	static long delay;
	static const int NPARAM = 30; 
	static const int NOBJ   = 2;
	static const double MINIMUM[];
	static const double MAXIMUM[];
	static const int OBJFUNC[];	
};

#endif
