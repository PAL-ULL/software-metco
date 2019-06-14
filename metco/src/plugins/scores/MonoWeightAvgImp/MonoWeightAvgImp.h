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

class MonoWeightAvgImp : public LocalScoreAlgorithm {
	public: 
		MonoWeightAvgImp() { optDirection = -1; }
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
		int restart;
		int terminated;
		int optDirection;
		int consideredValues;
		double pRandom;
		long double getScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue);
		vector< long double > refPoint;
		map< int, map < int, vector < long double > > > scores;
		map< int, map <int, int > > executions;
		map< int, map <int, vector<long double> > > improvements;
		map<int, map <int, int > > configToExecuting;
		map<int, pair<int, int> > islandToConfig;
};


