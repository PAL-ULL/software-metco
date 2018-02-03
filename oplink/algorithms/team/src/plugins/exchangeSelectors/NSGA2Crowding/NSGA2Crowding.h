/*****************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE 
 *    2012 
 *
 * DESCRIPTION
 ****************************************************************************************************/

#ifndef EXCHANGE_SELECTOR_NSGA2CROWDING_H
#define EXCHANGE_SELECTOR_NSGA2CROWDING_H

#include "ExchangeSelector.h"

class NSGA2Crowding : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
