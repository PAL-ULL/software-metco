/************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    November 2008
 *    
 * DESCRIPTION
 *    Each individual replace the nearest individual (variables space)
 * **********************************************************************************/


#ifndef EXCHANGE_SELECTOR_NEAREST_VARIABLES_H
#define EXCHANGE_SELECTOR_NEAREST_VARIABLES_H

#include "ExchangeSelector.h"

// Busca el elemento que más cercano esté en el dominio de las variables
// (usando la distancia euclidea) y lo sustituye por ese
class NearestVariables : public ExchangeSelector{
	void exchange(vector<Individual*> &migrated, vector<Individual*> &population);
};

#endif
