/* -----------------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------------- */

#ifndef __KNAPSACK_H__
#define __KNAPSACK_H__

#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Individual.h"
#include "utils.h"

const int CROSSOVER_UNIFORM = 0;
const int CROSSOVER_HUX = 1;

class Knapsack : public Individual {
   public:
    Knapsack() {}
    virtual ~Knapsack() {}
    // Reads a problem from file
    void readFile(const string filename);

    // Getters and Setters
    void inline setProfit(const int i, const float value) const { profits[i] = value; }
    void inline setWeight(const int i, const float value) const { weights[i] = value; }
    float inline getProfit(const int i) const { return profits[i]; }
    float inline getWeight(const int i) const { return weights[i]; }

    // Initialization function
    bool init(const vector<string> &params);
    // Random generation of an individual
    void restart(void);
    // Repairs an individual with the capacity restriction
    void repair(void);
    // Calculates the max profit/weight ratio for each item in increasing order
    void ratios(void);
    // Quicksort algorithm to order the max profit/weight ratio vector
    void quicksort(vector<int> &deleteOrder, vector<float> &maxRatio, int begin, int end);
    // Checks all capacities restrictions
    bool checkCapacity(void);
    // Individual evaluation and selection
    void evaluate(void);
    Individual *clone(void) const;
    // Ranges
    double inline getMaximum(const int i) const { return 1; }
    double inline getMinimum(const int i) const { return 0; }
    unsigned int inline getOptDirection(const int i) const { return MAXIMIZE; }
    unsigned int type_crossover;
    void crossover_HUX(Individual *ind);

   private:
    // Genetic Operators
    void dependentCrossover(Individual *ind);
    void dependentMutation(double pm);
    // Number of items
    static int nItems;
    // Profits, weights, capacities and max profit/weight ratios, respectively
    static vector<float> profits;
    static vector<float> weights;
    static float capacity;
    static vector<int> deleteOrder;
    static string filename;
    // Constants
    static const int ARGS;
};

#endif
