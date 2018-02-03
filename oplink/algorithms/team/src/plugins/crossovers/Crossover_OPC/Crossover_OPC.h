/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo Gonzalez
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef CROSSOVER_OPC_H
#define CROSSOVER_OPC_H

#include "Crossover.h"

class Crossover_OPC : public Crossover {
	void crossover(Individual *ind1, Individual *ind2);
};

#endif
