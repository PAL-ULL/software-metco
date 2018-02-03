#ifndef __MinimizationGenericIndividual_H__
#define __MinimizationGenericIndividual_H__

#include "Individual.h"
#include <math.h>

class MinimizationGenericIndividual : public Individual{
public:
	// Initialization method
	bool init (const vector<string> &params);

	// Genetic Operators
	void mutation (double pm);
	void crossover (Individual* ind);	

	// Evaluation
	void evaluate (void);

	// Ranges
	double inline getMaximum(const int i) const { return 1; }
	double inline getMinimum(const int i) const { return 0; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

	// Clone
	Individual* clone (void) const;
};

#endif
