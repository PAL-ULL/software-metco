/***********************************************************************************
 * AUTHORS
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef CROSSOVER_HUX_H
#define CROSSOVER_HUX_H

#include "Crossover.h"
#include "utils.h"

class Crossover_HUX : public Crossover {
	void crossover(Individual *ind1, Individual *ind2);
};

#endif
