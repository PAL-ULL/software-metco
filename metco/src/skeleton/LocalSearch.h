/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *
 * 	DESCRIPTION
 *   Plugin base class to represent a local search
 *********************************************************************************************/

#ifndef LOCAL_SEARCH_H 
#define LOCAL_SEARCH_H

#include <string>
#include <vector>
#include "Individual.h"
#include "Plugin.h"
#include "MOFront.h"

using namespace std;

class Individual;

class LocalSearch : public Plugin {
	public:
		virtual void localSearch(vector<Individual *> *population, MOFront *currentSolution) = 0;
};

LocalSearch *getLocalSearch(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError);
#endif
