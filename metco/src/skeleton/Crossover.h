/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *
 * DATE
 *    July 2008
 *
 * DESCRIPTION
 *   Plugin base class to represent crossovers between two individuals.
 *   The method crossover must be implemented. It receives two individuals ind1, and ind2,
 *   and the offspring individuals must overwrite them
 *********************************************************************************************/

#ifndef CROSSOVER_H 
#define CROSSOVER_H

#include <string>
#include "Individual.h"
#include "Plugin.h"

using namespace std;

class Individual;

class Crossover : public Plugin {
	public:
		virtual void crossover(Individual *ind1, Individual *ind2) = 0;
};

Crossover *getCrossover(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError);
#endif
