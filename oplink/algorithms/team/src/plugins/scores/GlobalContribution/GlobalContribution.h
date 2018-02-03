#ifndef __GLOBAL_CONTRIBUTION_H__
#define __GLOBAL_CONTRIBUTION_H__

#include <map>
#include "GlobalScoreAlgorithm.h"

class GlobalContribution : public GlobalScoreAlgorithm {
	public:
		virtual ~GlobalContribution() { finished = 0;}
		GlobalContribution() {}
		long double score(const MOFront *front);
		long double estimateScore(const int alg, const int conf, const double rndValue);
		void updateScore(const int alg, const int conf, const MOFront *globalSolution, const int inserted);
		bool init(const vector<string> &params);
		void begins(const int algId, const int confId, const int islandId); 
		void ends(const int algId, const int confId, const int islandId);
		int getDirection() const { return MAXIMIZE; }
	private:
		double pRandom;
		int consideredValues;
		int finished;
		map < int, map <int, int> > configToExecuting;
		map< int, map < int, vector < long double > > > scores;
};

#endif
