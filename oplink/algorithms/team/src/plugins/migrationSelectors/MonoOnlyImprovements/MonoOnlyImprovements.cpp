/*******************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    August 2008
 *******************************************************************************************/

#include "MonoOnlyImprovements.h"
#include "utils/utils.h"

MonoOnlyImprovements::MonoOnlyImprovements() : MigrationSelector(SELECT_FROM_ARCHIVE) {
	migrationPerformed = false;
}

void MonoOnlyImprovements::select(const vector<Individual*> &archive, int numberIndividual, vector<int> &selected){
	int bestIndex = 0;
	for (int i = 1; i < archive.size(); i++){
		if (((archive[i]->getInternalOptDirection(0) == MAXIMIZE) && (archive[i]->getObj(0) > archive[bestIndex]->getObj(0))) ||
			 ((archive[i]->getInternalOptDirection(0) == MINIMIZE) && (archive[i]->getObj(0) < archive[bestIndex]->getObj(0)))){
			 	bestIndex = i;
		}
	}
	if ((!migrationPerformed) || 
			 ((archive[bestIndex]->getInternalOptDirection(0) == MAXIMIZE) && (archive[bestIndex]->getObj(0) > bestMigrated)) ||
			 ((archive[bestIndex]->getInternalOptDirection(0) == MINIMIZE) && (archive[bestIndex]->getObj(0) < bestMigrated))){
		selected.push_back(bestIndex);
		migrationPerformed = true;
		bestMigrated = archive[bestIndex]->getObj(0);
	}
}
