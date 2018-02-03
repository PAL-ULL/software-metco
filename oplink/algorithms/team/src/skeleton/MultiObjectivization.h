/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *   Eduardo Segredo Gonzalez
 *
 * DATE
 *    November 2010
 *
 * DESCRIPTION
 *   Plugin base class to represent multiobjectivization of individuals.
 *   The method convertToMultiObjective must be implemented. It receives the current population
 *   and the index for the new calculated objective. It must calculate the objective and store it
 *   in each individual. 
 *********************************************************************************************/

#ifndef MULTIOBJECTIVIZATION_H 
#define MULTIOBJECTIVIZATION_H

using namespace std;

#include <vector>
#include <string>
#include "Individual.h"
#include "Plugin.h"

class Individual;

class MultiObjectivization : public Plugin {
	public:
		virtual void convertToMultiObjective(vector<Individual *> *parents, vector<Individual *> *offSprings, vector<Individual *> *reference, int ordObj, int numGeneration) = 0;
		virtual int getOptDirection() = 0;
		virtual void initiallization(Individual *ejInd) { }
};

MultiObjectivization *getMultiObjectivization(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError);
#endif
