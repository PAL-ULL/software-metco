#include "GlobalAlways.h"

void GlobalAlways::loadIslandInitPop(const int previousAlg, const int previousConf, const int newAlg, const int newConf, const vector< Individual* > *previousInit, const vector< Individual* > *previousEnd, const vector< Individual* > *global, vector < Individual* > &newPopulation, vector<int> &origin) const {
	for (int i = 0; i < global->size(); i++){
		newPopulation.push_back((*global)[i]);
		origin.push_back(GLOBAL);
	}
}
