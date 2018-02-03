/***********************************************************************************
 * AUTORES
 *   Gara Miranda Valladares 
 *   Carlos Segura González
 ***********************************************************************************/


#include <vector>
#include <map>
#include "LocalScoreAlgorithm.h"
#include "MOFront.h"

using namespace std;

class HVWeightAvg : public LocalScoreAlgorithm {
	public: 
		long double score(MOFront *);
		long double estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue);
		void updateScore(const long double breakFrom, const long double arriveTo, int alg, int conf, int islandId); 
		bool init(const vector<string> &params) ;
		void begins(const int algId, const int confId, const int islandId); 
		void ends(const int algId, const int confId, const int islandId);
		int getDirection() const { return MAXIMIZE; }
	private:
		int consideredValues;
		double pRandom;
		long double getScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue);
		vector< long double > refPoint;
		map< int, map < int, vector < pair <long double, long double > > > > scores;
		map<int, map <int, int > > configToExecuting;
		map<int, pair<int, int> > islandToConfig;
		map<int, int> islandExecutions;
};


