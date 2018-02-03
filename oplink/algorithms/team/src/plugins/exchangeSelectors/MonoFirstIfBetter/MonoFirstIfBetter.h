/********************************************************************************************
 * AUTHOR
 *    Carlos Segura Gonzalez
 *
 * DATE
 *    January 2009
 *    
 * DESCRIPTION
 *   If the migrated individual is better than population[0], then it put the migrated 
 *   individual in population[0] and population[1].
 *   It was designed to be used with VarPopSizeEA, because the algorithm store the best
 *   found individual in pop[0], but was implemented externally because other algorithms
 *   could use the same
 * *******************************************************************************************/

#ifndef EXCHANGE_SELECTOR_MONOFIRSTIFBETTER_H
#define EXCHANGE_SELECTOR_MONOFIRSTIFBETTER_H

#include "ExchangeSelector.h"

class MonoFirstIfBetter : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
