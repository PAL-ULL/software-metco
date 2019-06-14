/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    April 2010
 *
 * DESCRIPTION
 * 	When the configuration changes, it takes the individuals from the Global Solution. Otherwise,
 * 	it takes from the last population
 ************************************************************************************************/


#ifndef GLOBAL_IN_CHANGES_H
#define GLOBAL_IN_CHANGES_H

#include "Individual.h"
#include "InitPopIslandLoader.h"

#include <vector>

using namespace std;

class GlobalInChanges : public InitPopIslandLoader {
	public:
		void loadIslandInitPop(const int previousAlg, const int previousConf, const int newAlg, const int newConf, const vector< Individual * > *previousInit, const vector< Individual * > *previousEnd, const vector< Individual * > *global, vector <Individual * > &newPopulation, vector<int> &origin) const;
		bool useGlobal() const { return true; }
		bool usePreviousEnd() const { return true; }
};

#endif
