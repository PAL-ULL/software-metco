#include <vector>
#include <map>
#include "LocalScoreAlgorithm.h"
#include "MOFront.h"

using namespace std;

class MonoExpReg : public LocalScoreAlgorithm {
	public: 
		MonoExpReg() { optDirection = -1; penalized = 0; }
		long double score(MOFront *);
		long double estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue);
		void updateScore(const long double breakFrom, const long double arriveTo, int alg, int conf, int islandId); 
		bool init(const vector<string> &params) ;
		void printHistory();
		void begins(const int algId, const int confId, const int islandId); 
		void ends(const int algId, const int confId, const int islandId);
		int getDirection() const { 
			if (optDirection == -1) { 
				cout << "Error Interno. Direccion no fijada en modulo Score" << endl;
			}
			return optDirection; 
		}
	private:
		int optDirection;
		int consideredConfigurations;
		double penalized;
		long double getScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue);
		vector< long double > refPoint;
		map< int, map < int, vector < pair <long double, long double > > > > scores;
		map< int, map <int, int > > penalizations;
		map< int, map <int, int > > executions;
		map<int, map <int, int > > configToExecuting;
		map<int, pair<int, int> > islandToConfig;
};


