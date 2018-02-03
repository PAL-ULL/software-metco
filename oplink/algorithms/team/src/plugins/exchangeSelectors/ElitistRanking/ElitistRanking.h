/*****************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE 
 *    March 2009
 *
 * DESCRIPTION
 *    Rank all Pareto Fronts and replace individuals from the "worst" ranked front(s) with the 
 *    migrated individuals
 ****************************************************************************************************/

#ifndef EXCHANGE_SELECTOR_ELITISTRANKING_H
#define EXCHANGE_SELECTOR_ELITISTRANKING_H

#include "ExchangeSelector.h"

class ElitistRanking : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
