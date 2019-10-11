/* -----------------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------------- */

#ifndef __KNAPSACK_H__
#define __KNAPSACK_H__

#include <math.h>
#include <algorithm>  // std::sort
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "Individual.h"
#include "utils.h"

// Tupla para ordenar los elementos por su eficiencia
typedef pair<int, float> item_efficiency;

class Knapsack : public Individual {
 public:
  Knapsack() {}
  virtual ~Knapsack() {}
  // Reads a problem from file
  void readFile(const string filename);

  // Getters and Setters
  void inline setProfit(const int i, const float value) const {
    profits[i] = value;
  }
  void inline setWeight(const int i, const float value) const {
    weights[i] = value;
  }
  float inline getProfit(const int i) const { return profits[i]; }
  float inline getWeight(const int i) const { return weights[i]; }

  // Initialization function
  bool init(const vector<string> &params);
  // Random generation of an individual
  void restart(void);
  // Repairs an individual with the capacity restriction
  void repair(void);
  // Calculates the max profit/weight ratio for each item in increasing order
  void computeEfficiency(void);
  // Checks all capacities restrictions
  bool checkCapacity(void);
  // Individual evaluation and selection
  void evaluate(void);
  Individual *clone(void) const;
  // Ranges
  double inline getMaximum(const int i) const { return 1; }
  double inline getMinimum(const int i) const { return 0; }
  unsigned int inline getOptDirection(const int i) const { return MAXIMIZE; }

 private:
  // Genetic Operators
  void dependentMutation(double pm);
  // Number of items
  static int nItems;
  // Profits, weights, capacities and max profit/weight ratios, respectively
  static vector<float> profits;
  static vector<float> weights;
  static float capacity;
  // Array que tiene los indices de los elementos en funcion de su ratio
  // profit/weight
  static vector<item_efficiency> items_efficiency;
  static string filename;
  // Constants
  static const int ARGS;
};
bool sortByEfficiencyDesc(const pair<int, int> &firstPair,
                          const pair<int, int> &secondPair);
#endif
