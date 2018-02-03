/*******************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    August 2008
 *******************************************************************************************/

#include "RandomFromArchive.h"
#include "utils/utils.h"

RandomFromArchive::RandomFromArchive() : MigrationSelector(SELECT_FROM_ARCHIVE){
}

void RandomFromArchive::select(const vector<Individual*> &archive, int numberIndividual, vector<int> &selected){
	int generatedNumbers = (archive.size() > numberIndividual)?numberIndividual:archive.size();
	generateRandomValues(0, archive.size()-1, generatedNumbers, selected);
}


