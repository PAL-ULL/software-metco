/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo González
 *   - Carlos Segura González
 * 
 * FECHA
 *    Febrero 2011
 *
 * DESCRIPCION
 * Shifted Rastrigin's Function (F4)
 * Introduced in "Test Suite for the Special Issue of Soft Computing on Scalability
 * of Evolutionary Algorithms and other Metaheuristics for Large Scale Continuous
 * Optimization Problems".
 * ********************************************************************************/

#ifndef __Mono_F4_H__
#define __Mono_F4_H__

#include "Individual.h"



class Mono_F4 : public Individual {
public:

	// Initialization method
	bool init (const vector<string> &params);

	// Evaluation
	void evaluate (void);

	// Ranges
	double inline getMaximum(const int i) const { return 5; }
	double inline getMinimum(const int i) const { return -5; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

	// Clone
	Individual* clone (void) const;

private:
	// Constants
	static const int NOBJ   = 1;
};

#endif
