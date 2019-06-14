/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    April 2010
 *
 * DESCRIPTION
 * 	Always takes the individuals from the Global Solution
 ************************************************************************************************/


#ifndef GLOBAL_ALWAYS_H
#define GLOBAL_ALWAYS_H

#include "Individual.h"
#include "InitPopIslandLoader.h"

#include <vector>

using namespace std;

class GlobalAlways : public InitPopIslandLoader {
	public:
		void loadIslandInitPop(const int previousAlg, const int previousConf, const int newAlg, const int newConf, const vector< Individual * > *previousInit, const vector< Individual * > *previousEnd, const vector< Individual * > *global, vector <Individual * > &newPopulation, vector<int> &origin) const;
		bool useGlobal() const { return true; }
};

#endif
