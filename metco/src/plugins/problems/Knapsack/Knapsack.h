/* -----------------------------------------------------------------------------
 *  One-dimensional Knapsack Problem implementation
 *  Implementacion del 0-1 Knapsack Problem unidimensional.
 *
 *
 *  @author: Alejandro Marrero Diaz (alu0100825008@ull.edu.es)
 *  @date: 2019
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
  void readFile(const string filename);
  bool init(const vector<string> &params);
  void restart(void);
  void repair(void);
  void computeEfficiency(void);
  bool checkCapacity(void);
  void evaluate(void);
  Individual *clone(void) const;

  // Rango del espacio de busqueda de cada variable: [0, 1]
  double inline getMaximum(const int i) const { return 1; }
  double inline getMinimum(const int i) const { return 0; }
  unsigned int inline getOptDirection(const int i) const { return MAXIMIZE; }
  // Getters and Setters
  void inline setProfit(const int i, const float value) const {
    profits[i] = value;
  }
  void inline setWeight(const int i, const float value) const {
    weights[i] = value;
  }
  float inline getProfit(const int i) const { return profits[i]; }
  float inline getWeight(const int i) const { return weights[i]; }

 private:
  void dependentMutation(double pm);
  void roundVariables();

 private:
  static int nItems;
  static vector<float> profits;
  static vector<float> weights;
  static float capacity;
  static vector<item_efficiency> items_efficiency;
  static string filename;
  static const int ARGS;
};
bool sortByEfficiencyDesc(const pair<int, int> &firstPair,
                          const pair<int, int> &secondPair);
#endif
