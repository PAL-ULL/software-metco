/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *
 * 	DESCRIPTION
 *   Plugin base class to represent a mutation 
 *********************************************************************************************/

#ifndef MUTATION_H 
#define MUTATION_H

#include <string>
#include <vector>
#include "Individual.h"
#include "Plugin.h"

using namespace std;

class Individual;

class Mutation : public Plugin {
	public:
		virtual void mutation(Individual *ind, const double pm) = 0;
};

Mutation *getMutation(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError);
#endif
