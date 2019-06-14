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
 *    Individuo para la resolucion del problema de Benchmark DTLZ1.
 *    
 *    El problema DTLZ1 esta definido en:
 *     "Scalable Test Problems for Evolutionary Multi-Objective Optimization"
 *      Kalyanmoy Deb, Lothar Thiele, Marco Laumanns and Eckart Zitzler 
 *      TIK-Technical Report No. 112
 *      Institut fur Technische Informatik und Kommunikationsnetze
 *      ETH Zurich Gloriastrasse 35., ETH-Zentrum, CH-8092, Zurich, Switzerland
 *      July 17, 2001
 *    
 *    El problema DTLZ1 tiene M objetivos y un frente de Pareto óptimo lineal.
 *    El problema consiste en: 
 *       Minimizar f1(x)      = 1/2 x_1 x_2 ... x_(m-1) (1 + g(x_m))
 *       Minimizar f2(x)      = 1/2 x_1 x_2 ... (1- x_(m-1)) (1 + g(x_m))
 *                 ...            ...
 *                 ...            ...
 *                 ...            ...
 *       Minimizar f_(m-1)(x) = 1/2 x_1 (1 - x_2) (1 + g(x_m))
 *       Minimizar f_m(x)     = 1/2 (1 - x1) (1 + g(x_m))
 *       sujeto a que 0 <= x_i <= 1 para i = 1, 2, ..., n
 * 
 *    La función g implementada es la sugerida en el paper de referencia.
 *    The functional g(x_m) requires |x_m| = k variables and must take any function 
 *    with g >= 0. The Pareto-optimal solution corresponds to x_m = 0 and the objective 
 *    function values lie on a linear hyper-plane. 
 *    A value of k = 5 is suggested here. In the above problem, the total number of 
 *    variables is n = M + k - 1. The dificulty in this problem is to converge to the
 *    hyper-plane. 
 * 
 *    Se han definido los metodos requeridos por la clase Individual: 
 *     - mutation
 *     - crossover 
 *     - evaluate
 *     - init 
 *     - clone
 *
 *    Para más información acerca de la especificación de un problema, referirse a
 *    al fichero Individual.h
 *
 * ********************************************************************************/


#ifndef __INDIVIDUALDTLZ1_H__
#define __INDIVIDUALDTLZ1_H__


#include <iostream>
#include <math.h>

#include "Individual.h"


class DTLZ1 : public Individual {
public:
	// Initialization function
	bool init (const vector<string> &params);
	
	
	// Individual evaluation and selection
	void evaluate (void);
	
	// Clone
	Individual* clone (void) const;
	
	// Ranges
	double inline getMaximum(const int i) const { return 1; }
	double inline getMinimum(const int i) const { return 0; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

private:
	// Problem constants
	static const int k = 5;
};

#endif
