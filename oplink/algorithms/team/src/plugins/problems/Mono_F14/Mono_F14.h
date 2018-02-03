/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo González
 *   - Carlos Segura González
 * 
 * FECHA
 *    Febrero 2011
 *
 * DESCRIPCION
 * F14
 * Introduced in "Test Suite for the Special Issue of Soft Computing on Scalability
 * of Evolutionary Algorithms and other Metaheuristics for Large Scale Continuous
 * Optimization Problems".
 * ********************************************************************************/

#ifndef __Mono_F14_H__
#define __Mono_F14_H__

#include "Individual.h"

class Mono_F14 : public Individual {
public:

	// Initialization method
	bool init (const vector<string> &params);

	// Evaluation
	void evaluate (void);

	// Ranges
	double inline getMaximum(const int i) const { return 5; }
	double inline getMinimum(const int i) const { return -5; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }
  
  void inline setMns(const double &value) { mns = value; }
  double inline getMns() const { return mns; }

	// Clone
	Individual* clone (void) const;

private:
  // Auxiliar methods
  void divideFunctions(double *part1, double *part2, int *psize1, int *psize2);
  long double Extended_f_10NoDesplazamiento(int dim, double *x);
  long double f_10(double x, double y);
  long double Shifted_Rastrigin(int dim , const double *x);
 
	// Constants
	static const int NOBJ   = 1;

  // Parameters
  double mns;
};

#endif
