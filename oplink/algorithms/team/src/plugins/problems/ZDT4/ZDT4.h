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
 *    Individuo para la resolucion del problema de Benchmark ZDT4.
 *    El problema ZDT4 esta definido en:
 *       Comparison of Multiobjective Evolutionary Algorithms: Empirical Results
 *       Zitzler, E. and Deb, K. and Thiele, L.
 *    Consiste en:
 *       Minimizar (f1(x1), f2(x))
 *       sujeto a: f2(x) = g(x2,...,xm)*h(f1(x1),g(x2,...,xm))
 *       donde:    
 *         x = (x1,...,xm)
 *         m = 10
 *         f1(x1) = x1
 *         g(x2,...xm) = 1 + 10*(m-1) + SUM[i=2:m][xi^2 - 10 cos(4*PI*xi)]
 *         h(f1,g) = 1 - sqrt(f1/g)
 *         x1 en [0,1]
 *         x2,...xm en [-5,5]
 *
 *     Se han definido los metodos requeridos por Individual: init, mutation,
 *     crossover, evaluate y clone
 * ********************************************************************************/

#ifndef __INDIVIDUALZDT4_H__
#define __INDIVIDUALZDT4_H__

#include "Individual.h"
#include <math.h>

class ZDT4 : public Individual{
public:
	// Initialization method
	bool init (const vector<string> &params);


	// Evaluation
	void evaluate (void);

	// Clone
	Individual* clone (void) const;

	// Ranges
	double inline getMaximum(const int i) const { return MAXIMUM[i]; }
	double inline getMinimum(const int i) const { return MINIMUM[i]; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

private:
	static long delay;
	// Constants
	static const int NPARAM = 10; 
	static const int NOBJ   = 2;
	static const double MINIMUM[];
	static const double MAXIMUM[];
	static const int OBJFUNC[];	
};

#endif
