/*******************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    March 2010 
 *
 * DESCRIPTION
 *******************************************************************************************/

#ifndef MONOONLY_IMPROVEMENTS_H
#define MONOONLY_IMPROVEMENTS_H

#include "Individual.h"
#include "MOFront.h"
#include "MigrationSelector.h"

#include <vector>

using namespace std;

class MultiOnlyImprovements : public MigrationSelector {
	public:
		MultiOnlyImprovements();
		void select(const vector<Individual*> &pop, int numberIndividual, vector<int> &selected);
	private:
		MOFront *previouslyMigrated;
};

#endif
