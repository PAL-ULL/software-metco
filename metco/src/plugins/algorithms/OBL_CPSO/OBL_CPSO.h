#ifndef _OBL_PSO_H_
#define _OBL_PSO_H_

#include "Individual.h"
#include "EA.h"
#include <vector>
#include <random>
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;

class OBL_CPSO : public EA {
 public:
	bool init(const vector<string>& params);
	void getSolution(MOFront* p);
	void printInfo(ostream& os) const;
	void setGamma(const double gamma) { this->gamma = gamma; };
	double getGamma() { return gamma; };

 private:
	void runGeneration();
	void initVelocity();
	void shufflePop();
	void calculateMean();
	void compete();
	int compete1(int r1, int r2, int r3);
	int compete2(int r1, int r2, int r3);
	void update(int winner, int neutral, int loser);
	void sortPopulation();
	
	Individual* mean;
	double gamma;
	vector<int> randomPop;
	vector<vector<double>> velocity;
};

bool sortByObj0(const Individual* i1, const Individual* i2);

#endif
