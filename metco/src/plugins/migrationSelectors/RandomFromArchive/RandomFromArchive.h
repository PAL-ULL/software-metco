/*******************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    August 2008
 *
 * DESCRIPTION
 *    It selects random individuals from the current archive (island archive)
 *******************************************************************************************/

#ifndef RANDOM_FROM_ARCHIVE_H
#define RANDOM_FROM_ARCHIVE_H

#include "Individual.h"
#include "MigrationSelector.h"

#include <vector>

using namespace std;

class RandomFromArchive : public MigrationSelector {
	public:
		RandomFromArchive();
		void select(const vector<Individual*> &pop, int numberIndividual, vector<int> &selected);
};

#endif
