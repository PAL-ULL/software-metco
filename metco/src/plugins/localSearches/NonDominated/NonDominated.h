/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    May 2010
 *
 * DESCRIPTION
 *    
 ************************************************************************************************/


#ifndef __RANDOMWEIGHTS_H__
#define __RANDOMWEIGHTS_H__

#include <vector>

#include "Individual.h"
#include "LocalSearch.h"

using namespace std;

class NonDominated : public LocalSearch {
	public:
		void localSearch(vector <Individual*> *poblacion, MOFront *currentSolution);
		bool init(const vector<string> &params);
	private:
		int maxIterations;
		double pLS;
};
#endif
