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
 *    Individuo para la resolucion del problema de Benchmark DTLZ3.
 *    
 *    El problema DTLZ3 esta definido en:
 *     "Scalable Test Problems for Evolutionary Multi-Objective Optimization"
 *      Kalyanmoy Deb, Lothar Thiele, Marco Laumanns and Eckart Zitzler 
 *      TIK-Technical Report No. 112
 *      Institut fur Technische Informatik und Kommunikationsnetze
 *      ETH Zurich Gloriastrasse 35., ETH-Zentrum, CH-8092, Zurich, Switzerland
 *      July 17, 2001
 *    
 *    El problema DTLZ3 consiste en: 
 *       Minimizar f1(x)      = (1 + g(x_m)) cos(x_1 pi/2) cos(x_2 pi/2) ... cos(x_(m-2) pi/2) cos(x_(m-1) pi/2)
 *       Minimizar f2(x)      = (1 + g(x_m)) cos(x_1 pi/2) cos(x_2 pi/2) ... cos(x_(m-2) pi/2) sen(x_(m-1) pi/2)
 *       Minimizar f3(x)      = (1 + g(x_m)) cos(x_1 pi/2) cos(x_2 pi/2) ... sen(x_(m-2) pi/2) 
 *                 ...            ...
 *                 ...            ...
 *                 ...            ...
 *       Minimizar f_(m-1)(x) = (1 + g(x_m)) cos(x_1 pi/2) sen(x_2 pi/2)
 *       Minimizar f_m(x)     = (1 + g(x_m)) sen(x_1 pi/2) 
 *       sujeto a que 0 <= x_i <= 1 para i = 1, 2, ..., n
 * 
 *    Además, g(X_M) = 100 [ |x_M| + sum(x_i - 0.5)^2 - cos(20pi(x_i - 0.5))]
 *    El conjunto de soluciones óptimas se corresponde con X_M* = 0.5
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


#ifndef __INDIVIDUALDTLZ3_H__
#define __INDIVIDUALDTLZ3_H__


#include <iostream>
#include <math.h>

#include "Individual.h"


class DTLZ3 : public Individual {
public:
	// Initialization function
	bool init (const vector<string> &params);


	// Individual evaluation and selection
	void evaluate (void);
	
	// Clonación de un individuo
	Individual* clone (void) const;

	// Ranges
	double inline getMaximum(const int i) const { return 1; }
	double inline getMinimum(const int i) const { return 0; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

private:
	// Problem constants 
	static const int k = 10;
};

#endif
