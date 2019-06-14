/***********************************************************************************
 * AUTOR
 *   - Francisco Jose Morales de la Rosa
 *
 * FECHA
 *    Mayo 2009
 *
 * DESCRIPCION
 *    Individuo para la resolucion del problema de Benchmark R2.
 *    El problema R2 esta definido en:
 *       Rotated Test Problems for Assessing the Performance of Multi-objective Optimization Algorithms
 *       Antony W. Iorio and Xiaodong Li
 *    Consiste en:
 *       Minimizar (f1(y), f2(y))
 *       sujeto a: f2(y) = g(y)*h(f1(y),g(y))
 *       donde:
 *         m = 10
 *         f1(y) = y1
 *         g(y) = 1 + 10*(m-1) + SUM[i=2:m][yi^2 - 10cos(pi*yi)]
 *         h(f1,g) = 1.0 + exp(-f1/g) + ((f1+1.0)/g) * sin(5*pi*f1)
 *         xi en [-1.0,1.0]
 *
 *     Se han definido los metodos requeridos por Individual: init, evaluate, clone,
 *     getMaximun, getMinimun y getOptDirection
 * ********************************************************************************/

#ifndef __R2_H__
#define __R2_H__

#include "Individual.h"
#include <math.h>

class R2 : public Individual {
public:
	// Initialization method
	bool init (const vector<string> &params);


	// Evaluation
	void evaluate (void);

	// Ranges
	double inline getMaximum(const int i) const { return 1.0; }
	double inline getMinimum(const int i) const { return -1.0; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

	// Clone
	Individual* clone (void) const;

private:
	static long delay;
	// Constants
	static const int NPARAM = 10; 
	static const int NOBJ   = 2;
};

#endif
