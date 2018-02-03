/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    May 2010
 *
 * DESCRIPTION
 *    For each individual a set of random weights are generated, and used to evaluate the 
 *    individual and its neighbours. It move to the best neighbour.
 *    It is repeated a maximum of maxIterations times.
 *    pLS determines the probability of performing the local search
 ************************************************************************************************/


#ifndef __RANDOMWEIGHTS_H__
#define __RANDOMWEIGHTS_H__

#include <vector>

#include "Individual.h"
#include "LocalSearch.h"

using namespace std;

class RandomWeights : public LocalSearch {
	public:
		void localSearch(vector <Individual*> *poblacion, MOFront *currentSolution);
		double calculateF( Individual *ind, vector <double> &weights);
		double calculateF( vector<double> &objectives, vector <double> &weights);
		bool init(const vector<string> &params);
	private:
		int maxIterations;
		double pLS;
};
#endif
