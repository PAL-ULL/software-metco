/************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *    Eduardo Segredo Gonzalez
 *
 * DATE
 *    July 2011
 *
 * DESCRIPTION
 *    For each migrated individual, the nearest individual (variable space) is 
 *    exchanged, if the migrated individual is better than any individual in
 *    the population
 * **********************************************************************************/

#ifndef EXCHANGE_SELECTOR_NEAREST_VARIABLESHAMMING_IFBESTMONO_H
#define EXCHANGE_SELECTOR_NEAREST_VARIABLESHAMMING_IFBESTMONO_H

#include "ExchangeSelector.h"

class NearestVariablesHammingIfBestMono : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
