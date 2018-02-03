/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo González
 *   - Carlos Segura González
 * 
 * FECHA
 *    Febrero 2011
 *
 * DESCRIPCION
 * Shifted Schwefel 1.2 Function (F8)
 * Introduced in "Test Suite for the Special Issue of Soft Computing on Scalability
 * of Evolutionary Algorithms and other Metaheuristics for Large Scale Continuous
 * Optimization Problems".
 *
 * Nota: en la primera implementación, los rangos estaban mal [-65536, 65536]. Por
 * eso los artículos publicados con esto hasta el 6/06/13, tienen errores mucho
 * más grandes de lo que se esperaría.
 * ********************************************************************************/

#ifndef __Mono_F8_H__
#define __Mono_F8_H__

#include "Individual.h"
#include <float.h>
#include <math.h>


class Mono_F8 : public Individual {
public:

	// Initialization method
	bool init (const vector<string> &params);

	// Evaluation
	void evaluate (void);

	// Ranges
	double inline getMaximum(const int i) const { return 65.536; }
	double inline getMinimum(const int i) const { return -65.536; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

	// Clone
	Individual* clone (void) const;

private:
	// Constants
	static const int NOBJ   = 1;
};

#endif
