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

class Uniform : public LocalScoreAlgorithm {
	public: 
		long double score(MOFront *);
		long double estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue);
		void updateScore(const long double breakFrom, const long double arriveTo, int alg, int conf, int islandId) { }
		bool init(const vector<string> &params) ;
		int getDirection() const { return MAXIMIZE; }
	private:
		vector< long double > refPoint;
};


