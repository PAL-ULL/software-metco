/**************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    October 2008
 *
 * DESCRIPTION
 *    The dominated individual, which is in the nearest position (objective space)
 *    is replaced
 **************************************************************************************/



#ifndef EXCHANGE_SELECTOR_ELITISTNEARESTOBJECTIVES_H
#define EXCHANGE_SELECTOR_ELITISTNEARESTOBJECTIVES_H

#include "ExchangeSelector.h"

class ElitistNearestObjectives : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
