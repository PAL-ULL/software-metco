#ifndef _CSO_H_
#define _CSO_H_

#include "Individual.h"
#include "EA.h"
#include <vector>
#include <random>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <functional>

using namespace std;

#define GLOBAL_SEARCH_SIMILARITY 0
#define GLOBAL_SEARCH_ORIGINAL 1
#define GLOBAL_SEARCH_NO 2

class CSO : public EA {
 public:
	bool init(const vector<string>& params);
	void getSolution(MOFront* p);
	void printInfo(ostream& os) const;

	void setGamma(const double gamma) { this->gamma = gamma; }
	double getGamma() { return gamma; }

 private:
	void runGeneration();
	void shufflePop();
	void calculateMean();
	void compete();
	void update(int winner, int loser);
	void sortPopulation();

  int maxVarValues;
  double varThreshold;
  vector<double> varValues;

	int globalSearchType;
	bool globalSearchRequired;

  // Global search selection
  void globalSearch();    

  // Original proposal
  void GlobalSearchGlobalNeighbourhood();
  // Similarity-based Global Search (SGS): Linear descending function
  void GlobalSearchGlobalNeighbourhood8();
  // Similarity-based Global Search (SGS): Linear ascending function
  void GlobalSearchGlobalNeighbourhood9();
  // Similarity-based Global Search (SGS): Considers variability of the population and a threshold value
  void GlobalSearchGlobalNeighbourhood10();
  // Similarity-based Global Search (SGS): Global Neighbourhood Path Search
  void globalNeighbourhoodPathSearch();
    
  bool sortByDistanceToBest(const Individual *i1, const Individual *i2, const Individual *best);

	double gamma;
  Individual* mean;
	vector<int> randomPop;
	vector<vector<double>> velocity;
};

bool sortByObj0(const Individual* i1, const Individual* i2);

#endif
