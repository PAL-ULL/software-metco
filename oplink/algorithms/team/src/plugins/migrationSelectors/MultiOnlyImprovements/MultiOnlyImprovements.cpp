/*******************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    August 2008
 *******************************************************************************************/

#include "MOFrontVector.h"
#include "SimpleIndividual.h"
#include "MultiOnlyImprovements.h"
#include "utils/utils.h"

MultiOnlyImprovements::MultiOnlyImprovements() : MigrationSelector(SELECT_FROM_ARCHIVE){
	previouslyMigrated = new MOFrontVector(NULL, false, true);
}

void MultiOnlyImprovements::select(const vector<Individual*> &archive, int numberIndividual, vector<int> &selected){
	for (int i = 0; i < archive.size(); i++){
		//SimpleIndividual *simp = new SimpleIndividual(0, archive[i]->getNumberOfObj());
		SimpleIndividual *simp = new SimpleIndividual(0, archive[i]->getNumberOfObj());
		for (int j = 0; j < archive[i]->getNumberOfObj(); j++){
			simp->setObj(j, archive[i]->getObj(j));
			simp->setOptDirection(j, archive[i]->getInternalOptDirection(j));
		}
		if (previouslyMigrated->insert(simp)){
			selected.push_back(i);
			if (selected.size() == numberIndividual)
				return;
		} else {
			delete simp;
		}
	}
}
