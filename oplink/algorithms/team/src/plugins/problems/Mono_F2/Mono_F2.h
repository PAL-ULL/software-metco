/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo González
 *   - Carlos Segura González
 * 
 * FECHA
 *    Febrero 2011
 *
 * DESCRIPCION
 * Shifted Schwefel's 2.21 Function (F2)
 * Introduced in "Test Suite for the Special Issue of Soft Computing on Scalability
 * of Evolutionary Algorithms and other Metaheuristics for Large Scale Continuous
 * Optimization Problems".
 * ********************************************************************************/

#ifndef __Mono_F2_H__
#define __Mono_F2_H__

#include "Individual.h"

class Mono_F2 : public Individual {
public:

	// Initialization method
	bool init (const vector<string> &params);

	// Evaluation
	void evaluate (void);

	// Ranges
	long double max(long double x, long double y) { return ((x > y)?x:y); }
	double inline getMaximum(const int i) const { return 100; }
	double inline getMinimum(const int i) const { return -100; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

	// Clone
	Individual* clone (void) const;

private:
	// Constants
	static const int NOBJ   = 1;
};

#endif
