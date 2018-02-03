/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo González
 *   - Carlos Segura González
 * 
 * FECHA
 *    Febrero 2011
 *
 * DESCRIPCION
 * F15
 * Introduced in "Test Suite for the Special Issue of Soft Computing on Scalability
 * of Evolutionary Algorithms and other Metaheuristics for Large Scale Continuous
 * Optimization Problems".
 * ********************************************************************************/

#ifndef __Mono_F15_H__
#define __Mono_F15_H__

#include "Individual.h"

class Mono_F15 : public Individual {
public:

	// Initialization method
	bool init (const vector<string> &params);

	// Evaluation
	void evaluate (void);

	// Ranges
	double inline getMaximum(const int i) const { return 10; }
	double inline getMinimum(const int i) const { return -10; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }
  
  void inline setMns(const double &value) { mns = value; }
  double inline getMns() const { return mns; }

	// Clone
	Individual* clone (void) const;

private:
  // Auxiliar methods
  void divideFunctions(double *s, double *part1, double *part2, int *psize1, int *psize2);
  long double f_BohachevskyNoDesplazamiento(int dim, double *s);
  long double f_Schwefel2_22NoDesplazamiento(int dim, double *s);
 
	// Constants
	static const int NOBJ   = 1;

  // Parameters
  double mns;
};

#endif
