/**************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    October 2008
 *
 * DESCRIPTION
 *    The dominated individual, which is in the nearest position (variables space)
 *    is replaced
 **************************************************************************************/

#ifndef EXCHANGE_SELECTOR_ELITIST_NEAREST_VARIABLES_H
#define EXCHANGE_SELECTOR_ELITIST_NEAREST_VARIABLES_H

#include "ExchangeSelector.h"

class ElitistNearestVariables : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
