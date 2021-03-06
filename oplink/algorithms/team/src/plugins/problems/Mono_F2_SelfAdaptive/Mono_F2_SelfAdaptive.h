/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo Gonz�lez
 *   - Carlos Segura Gonz�lez
 * 
 * FECHA
 *    Abril 2012
 *
 * DESCRIPCION
 * Shifted Schwefel's 2.21 Function (F2)
 * Introduced in "Test Suite for the Special Issue of Soft Computing on Scalability
 * of Evolutionary Algorithms and other Metaheuristics for Large Scale Continuous
 * Optimization Problems".
 * This version includes a parameter in the codification of the individual
 * ********************************************************************************/

#ifndef __Mono_F2_SELFADAPTIVE_H__
#define __Mono_F2_SELFADAPTIVE_H__

#include "Individual.h"

class Mono_F2_SelfAdaptive : public Individual {
public:

	// Initialization method
	bool init (const vector<string> &params);

	// Evaluation
	void evaluate (void);

	// Ranges
	long double max(long double x, long double y) { return ((x > y)?x:y); }
	double getMaximum(const int i) const;
	double getMinimum(const int i) const;
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

	// Clone
	Individual* clone (void) const;

private:
	// Constants
	static const int NOBJ   = 1;
};

#endif
