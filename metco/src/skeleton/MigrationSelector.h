/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *
 * DESCRIPTION
 *   Plugin base class to represent a migration selector 
 *********************************************************************************************/


#ifndef MIGRATION_SELECTOR_H
#define MIGRATION_SELECTOR_H

#include "Individual.h"
#include "Plugin.h"

#include <vector>

#define SELECT_FROM_ARCHIVE 0
#define SELECT_FROM_POPULATION 1

using namespace std;

class MigrationSelector : public Plugin {
	public:
		MigrationSelector(int type);
		int getType() const { return type; }
		virtual void select(const vector<Individual*> &pop, int numberIndividual, vector<int> &selected) = 0;
	private:
		int type;
};

MigrationSelector *getMigrationSelector(const string &pluginPath, const string &migSelectorName, const vector<string> &args, bool printError);
#endif
