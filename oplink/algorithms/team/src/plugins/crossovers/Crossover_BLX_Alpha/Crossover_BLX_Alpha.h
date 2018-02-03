/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo
 *   - Carlos Segura
 * 
 * *********************************************************************************/

/* 
Implementación del Blend Crossover con parámetro alpha

Para más detalles véase:

Takahashi M., Kita H.  (2001), A Crossover Operator Using Independent Component Analysis
for Real-Coded Genetic Algorithm, in Proceedings of the 2001 Congress on Evolutionary
Computation, pp. 643-649

Herrera F. , Lozano M. , Sánchez A.M.  (2003), A taxonomy for the Crossover Operator for
Real-Coded Genetic Algorithms: An Experimental Study, in International Journal of
Intelligent Systems, Wiley, vol. 18, pp. 309-338

Pseudocódigo en:

http://www.tomaszgwiazda.com/blendX.htm

Nota: En esta implementación el parámetro alpha vale 0.5
*/


#ifndef CROSSOVER_BLX_ALPHA_H
#define CROSSOVER_BLX_ALPHA_H

#include "Crossover.h"
#include "utils.h"

class Crossover_BLX_Alpha : public Crossover {
	void crossover(Individual *ind1, Individual *ind2);
};

#endif
