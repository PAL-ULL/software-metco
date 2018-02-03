/***********************************************************************************
 * AUTHORS
 *   - ANTONIO J. NEBRO
 *
 * NOTES
 *   JMetal Implementation
 * *********************************************************************************/

#ifndef CROSSOVER_SBX_H
#define CROSSOVER_SBX_H

#include "Crossover.h"

class Crossover_SBX2 : public Crossover {
	void crossover(Individual *ind1, Individual *ind2);
};

#endif
