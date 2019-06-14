/************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    November 2008
 *
 * DESCRIPTION
 *    For each migrated individual, the nearest individual (objective space) is 
 *    exchanged, if such individual is dominated by the migrated individual
 * **********************************************************************************/

#ifndef EXCHANGE_SELECTOR_NEAREST_OBJECTIVES_IFDOM_H
#define EXCHANGE_SELECTOR_NEAREST_OBJECTIVES_IFDOM_H

#include "ExchangeSelector.h"

class NearestObjectivesIfDom : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
