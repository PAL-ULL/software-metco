/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    April 2010
 *
 * DESCRIPTION
 * 	The initial configuration uses the last population of the previous configuration which was
 * 	executed on the island.
 ************************************************************************************************/


#ifndef PREVIOUS_END_ALWAYS_H
#define PREVIOUS_END_ALWAYS_H

#include "Individual.h"
#include "InitPopIslandLoader.h"

#include <vector>

using namespace std;

class PreviousEndAlways : public InitPopIslandLoader {
	public:
		void loadIslandInitPop(const int previousAlg, const int previousConf, const int newAlg, const int newConf, const vector< Individual * > *previousInit, const vector< Individual * > *previousEnd, const vector< Individual * > *global, vector <Individual * > &newPopulation, vector<int> &origin) const;
		bool usePreviousEnd() const { return true; }
};

#endif
