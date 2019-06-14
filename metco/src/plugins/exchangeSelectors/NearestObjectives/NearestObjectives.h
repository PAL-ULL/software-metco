/************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    November 2008
 *    
 * DESCRIPTION
 *    Each individual replace the nearest individual (objective space)
 * **********************************************************************************/

#ifndef EXCHANGE_SELECTOR_NEAREST_OBJECTIVES_H
#define EXCHANGE_SELECTOR_NEAREST_OBJECTIVES_H

#include "ExchangeSelector.h"

class NearestObjectives : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
