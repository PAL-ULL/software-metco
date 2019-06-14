/*********************************************************************************
 * AUTHORS
 *    Carlos Segura Gonzalez
 *
 * DATE
 *    November 2008
 *
 * DESCRIPTION
 *    A random individual of the population is selected
 *********************************************************************************/

#ifndef EXCHANGE_SELECTOR_RANDOM_H
#define EXCHANGE_SELECTOR_RANDOM_H

#include "ExchangeSelector.h"

//Escoge elementos aleatorios de la poblacion
class Random : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
