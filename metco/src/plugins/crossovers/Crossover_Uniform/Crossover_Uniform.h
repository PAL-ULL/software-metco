/***********************************************************************************
 * AUTHORS
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef CROSSOVER_UNIFORM_H
#define CROSSOVER_UNIFORM_H

#include "Crossover.h"

class Crossover_Uniform : public Crossover {
	void crossover(Individual *ind1, Individual *ind2);
};

#endif
