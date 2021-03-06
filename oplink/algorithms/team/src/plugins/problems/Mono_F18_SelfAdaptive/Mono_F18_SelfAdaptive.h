/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo Gonz�lez
 *   - Carlos Segura Gonz�lez
 * 
 * FECHA
 *    Abril 2012
 *
 * DESCRIPCION
 * Function F18
 * Introduced in "Test Suite for the Special Issue of Soft Computing on Scalability
 * of Evolutionary Algorithms and other Metaheuristics for Large Scale Continuous
 * Optimization Problems".
 * ********************************************************************************/

#ifndef __Mono_F18_SELFADAPTIVE_H__
#define __Mono_F18_SELFADAPTIVE_H__

#include "Individual.h"

class Mono_F18_SelfAdaptive : public Individual {
public:

	// Initialization method
	bool init (const vector<string> &params);

	// Evaluation
	void evaluate (void);

	// Ranges
	double getMaximum(const int i) const;
	double getMinimum(const int i) const;
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }
  
  void inline setMns(const double &value) { mns = value; }
  double inline getMns() const { return mns; }

	// Clone
	Individual* clone (void) const;

private:
  // Auxiliar methods
  void divideFunctions(double *part1, double *part2, int *psize1, int *psize2);
	long double Extended_f_10NoDesplazamiento(int dim, double *x);
	long double Shifted_Rastrigin( int dim , const double * x );
	long double f_10(double x, double y);
 
	// Constants
	static const int NOBJ   = 1;

  // Parameters
  double mns;
};

#endif
