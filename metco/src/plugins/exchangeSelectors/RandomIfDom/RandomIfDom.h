/************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    November 2008
 *
 * DESCRIPTION
 *    For each migrated individual, a random individual of the population is selected.
 *    If such individual is dominated by the migrated individual, it is exchanged.
 ************************************************************************************/


#ifndef EXCHANGE_SELECTOR_RANDOM_IFDOM_H
#define EXCHANGE_SELECTOR_RANDOM_IFDOM_H

#include "ExchangeSelector.h"

class RandomIfDom : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
