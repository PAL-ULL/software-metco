/*****************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE 
 *    March 2009
 *
 * DESCRIPTION
 *    Combine inmigrants with the current population, rank all Pareto fronts, and remove individuals
 *    from the "worst" ranked front(s)
 ****************************************************************************************************/

#ifndef EXCHANGE_SELECTOR_ELITIST100RANKING_H
#define EXCHANGE_SELECTOR_ELITIST100RANKING_H

#include "ExchangeSelector.h"

class Elitist100Ranking : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
