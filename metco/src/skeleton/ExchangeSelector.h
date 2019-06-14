/***********************************************************************************************
 * AUTHORS
 * - Carlos Segura González
 *
 * DESCRIPTION
 *   Plugin Base class to represent Exchange Selectors (Migration). 
 *   The method exchange receives the population and migration vectors and must decide
 *   which individuals are exchanged in the population.
 ***********************************************************************************************/


#ifndef EXCHANGE_SELECTOR_H
#define EXCHANGE_SELECTOR_H

#include "Plugin.h"
#include "Individual.h"

#include <vector>

using namespace std;

class ExchangeSelector : public Plugin {
	public:
		virtual void exchange(vector<Individual*> &migrated, vector<Individual*> &population) = 0;
};

ExchangeSelector *getExchangeSelector(const string &pluginPath, const string &pluginName, vector<string> &args, bool printError);
#endif
