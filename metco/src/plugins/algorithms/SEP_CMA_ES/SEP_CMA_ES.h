#ifndef _SEP_CMA_ES_H_
#define _SEP_CMA_ES_H_

#include "Individual.h"
#include "EA.h"
#include <vector>
#include <random>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <chrono>

using namespace std;

typedef struct {
	double value;
	int id;
} sortedvals;

bool sortByObj0(const Individual* i1, const Individual* i2);
bool compare(const sortedvals i1, const sortedvals i2);

class SEP_CMA_ES : public EA {
 public:
	bool init(const vector<string>& params);
	void getSolution(MOFront* p);
	void printInfo(ostream& os) const;

 private:
  // Methods
	void runGeneration();
	void sortPopulation();

	//double *getRestartInfo();
	//void setRestartInfo(double *);

	double minv(double a, double b);
	double maxv(double a, double b);
	void mysort();

  // Attributes
	int lambda, N, mu;
	double sigma, initSigma;
	double c1, cmu, ccov1_sep, ccovmu_sep, chiN, cs, damps, ccum, mueff;

  //m*n
  vector<vector<double>> arx;
	vector<vector<double>> arz;
	// n
	vector<double> diagD;
	vector<double> diagC;
	vector<double> pc;
	vector<double> ps;
	vector<double> xmean;
	vector<double> xold;
	// lambda, mu, nvectors
	vector<double> weights;
	vector<double> arfitness;
	vector<int> arindex;
	vector<sortedvals> arr_tmp;
  
  default_random_engine generator;
	uniform_real_distribution<double> uniformDist;
	normal_distribution<double> normalDist;

  Individual *best;
};

#endif
