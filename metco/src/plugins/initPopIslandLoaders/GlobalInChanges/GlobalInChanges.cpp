#include "GlobalInChanges.h"

void GlobalInChanges::loadIslandInitPop(const int previousAlg, const int previousConf, const int newAlg, const int newConf, const vector< Individual* > *previousInit, const vector< Individual* > *previousEnd, const vector< Individual* > *global, vector < Individual* > &newPopulation, vector<int> &origin) const {
	int originVectorId;
	const vector< Individual *> *originVector;
	const vector< Individual *> *deleteVector;
	if ((previousAlg == newAlg) && (previousConf == newConf)){
		originVectorId = PREVIOUSEND;
		originVector = previousEnd;
		deleteVector = global;
	} else {
		originVectorId = GLOBAL;
		originVector = global;
		deleteVector = previousEnd;
	}
	for (int i = 0; i < originVector->size(); i++){
		newPopulation.push_back((*originVector)[i]);
		origin.push_back(originVectorId);
	}
	for (int i = 0; i < deleteVector->size(); i++){
		delete (*deleteVector)[i];
	}
}
