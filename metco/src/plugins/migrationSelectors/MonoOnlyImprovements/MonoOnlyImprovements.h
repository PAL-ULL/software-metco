/*******************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    August 2008
 *
 * DESCRIPTION
 *    It save the best migrated individual, and only migrates individuals better that such
 *    individual
 *    It is for monoobjective optimization
 *******************************************************************************************/

#ifndef MONOONLY_IMPROVEMENTS_H
#define MONOONLY_IMPROVEMENTS_H

#include "Individual.h"
#include "MigrationSelector.h"

#include <vector>

using namespace std;

class MonoOnlyImprovements : public MigrationSelector {
	public:
		MonoOnlyImprovements();
		void select(const vector<Individual*> &pop, int numberIndividual, vector<int> &selected);
	private:
		bool migrationPerformed;
		double bestMigrated;
};

#endif
