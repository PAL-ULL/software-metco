/* -----------------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------------- */


#ifndef __KNAPII_H__
#define __KNAPII_H__


#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>

#include "Individual.h"


class KNAPII : public Individual {

	private:

	// Genetic Operators
	void dependentMutation(double pm);
	void dependentCrossover(Individual* ind);

	// Number of items and number of objectives (two in this case)
	static int nParam;
	static int nObj;
	
	// Profits, weights, capacities and profit/weight ratios, respectively
	static float *profit;
	static float *weight;
	static float capacity;
	static int *deleteOrder;
	static const int OBJFUNC[];	
	
	public:
	
	// Reads a problem from file
	void readFile(const string filename);

	// Initialization function
	bool init(const vector<string> &params);


	// Random generation of an individual
	void restart(void);

	// Repairs an individual with the capacity restriction
	void repair(void);

	// Calculates the profit/weight ratio for each item in increasing order
	void ratios(void);

	// Quicksort algorithm to order the profit/weight ratio vector
	void quicksort(int *deleteOrder, float *ratio, int begin, int end);

	// Checks capacity restriction
	bool checkCapacity(void);

	// Individual evaluation and selection
	void evaluate(void);
	
	Individual* clone(void) const;

	// Ranges
	double inline getMaximum(const int i) const { return 1; }
	double inline getMinimum(const int i) const { return 0; }
	unsigned int inline getOptDirection(const int i) const { return OBJFUNC[i]; }

};

#endif
