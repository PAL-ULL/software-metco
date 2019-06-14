/***************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    2008
 *    
 * DESCRIPTION
 *   Random dominated individuals are selected
 * *************************************************************************************/

#ifndef EXCHANGE_SELECTOR_ELITIST_RANDOM_H
#define EXCHANGE_SELECTOR_ELITIST_RANDOM_H

#include "ExchangeSelector.h"

class ElitistRandom : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
