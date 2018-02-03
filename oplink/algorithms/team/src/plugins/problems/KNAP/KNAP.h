/* -----------------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------------- */


#ifndef __KNAP_H__
#define __KNAP_H__


#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>

#include "utils.h"
#include "Individual.h"

#define CROSSOVER_UNIFORM 0
#define CROSSOVER_HUX 1

class KNAP : public Individual {

	private:
		// Genetic Operators
		void dependentCrossover(Individual* ind);
		void dependentMutation(double pm);

	// Number of items and number of knapsacks, respectively
	static int nParam;
	static int nObj;
	
	// Profits, weights, capacities and max profit/weight ratios, respectively
	static float *profit;
	static float *weight;
	static float *capacity;
	static int *deleteOrder;
	
	public:

	// Reads a problem from file
	void readFile(const string filename);

	// Setters
	void inline setProfit(const int i, const int j, const float value) const { profit[i * nParam + j] = value; }
	void inline setWeight(const int i, const int j, const float value) const { weight[i * nParam + j] = value; }
	
	// Getters
	float inline getProfit(const int i, const int j) const { return profit[i * nParam + j]; }
	float inline getWeight(const int i, const int j) const { return weight[i * nParam + j]; }

	// Initialization function
	bool init(const vector<string> &params);
	

	// Random generation of an individual
	void restart(void);

	// Repairs an individual with the capacity restriction
	void repair(void);

	// Calculates the max profit/weight ratio for each item in increasing order
	void ratios(void);

	// Quicksort algorithm to order the max profit/weight ratio vector
	void quicksort(int *deleteOrder, float *maxRatio, int begin, int end);

	// Checks all capacities restrictions
	bool checkCapacity(void);

	// Individual evaluation and selection
	void evaluate(void);
	
	Individual* clone(void) const;

	// Ranges
	double inline getMaximum(const int i) const { return 1; }
	double inline getMinimum(const int i) const { return 0; }
	unsigned int inline getOptDirection(const int i) const { return MAXIMIZE; }
	
	unsigned int type_crossover;
	
	void crossover_HUX (Individual *ind);

};

#endif
