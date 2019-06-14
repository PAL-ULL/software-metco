/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    April 2010
 *
 * DESCRIPTION
 * 	It considers the initial and last population of the last executed configuration. If an
 * 	individual of the last population is not worse (in a percentage fixed by the user) than
 * 	the initial individual which was in the same position, it is used the one of the last 
 * 	population.
 * 	Otherwise, the individual from the initial population is recovered.
 ************************************************************************************************/


#ifndef PREVIOUS_END_CHECKING_MONO_WORSENING_H
#define PREVIOUS_END_CHECKING_MONO_WORSENING_H

#include "Individual.h"
#include "InitPopIslandLoader.h"

#include <vector>

#define PERCENTAGE 0
#define ABSOLUTE 1
using namespace std;

class PreviousEndCheckingMonoWorsening : public InitPopIslandLoader {
	public:
		void loadIslandInitPop(const int previousAlg, const int previousConf, const int newAlg, const int newConf, const vector< Individual * > *previousInit, const vector< Individual * > *previousEnd, const vector< Individual * > *global, vector <Individual * > &newPopulation, vector<int> &origin) const;
		bool init(const vector<string> &params);
		bool usePreviousEnd() const { return true; }
		bool usePreviousInit() const { return true; }
	private:
		double acceptedWorsening;
		int worseningType;
};

#endif
