/* -----------------------------------------------------------------------------
 *  One-dimensional Knapsack Problem implementation
 *
 *  @Author: Alejandro Marrero Diaz (alu0100825008@ull.edu.es)
 *
 * -------------------------------------------------------------------------- */

#include "Knapsack.h"
#include <algorithm>

// Number of items, profits, weights and capacity
int Knapsack::nItems;
vector<float> Knapsack::profits;
vector<float> Knapsack::weights;
float Knapsack::capacity;
vector<int> Knapsack::deleteOrder;

const int Knapsack::ARGS = 1;

// Problem Initialization
bool Knapsack::init(const vector<string> &params) {
  // Check for the problem parameters
  if (params.size() != ARGS) {
    cout << "Error Knapsack: Incorrect parameters " << endl;
    cout << "<instance>" << endl;
    return false;
  }
  string filename = params[0];
  setNumberOfObj(1);
  // Reads a problem from file
  readFile(filename);
  return true;
}

// Reads a problem from file
void Knapsack::readFile(const string filename) {
  ifstream input(filename.c_str());
  string readLine;
  input >> nItems;
  input >> capacity;
  profits.resize(nItems);
  weights.resize(nItems);
  deleteOrder.resize(nItems);
  setNumberOfVar(nItems);
  float weight, profit;
  for (int i = 0; i < nItems; i++) {
    deleteOrder[i] = i;
    input >> weight >> profit;
    weights[i] = weight;
    profits[i] = profit;
  }
  input.close();
}

// Genetic Operators - Binary Mutation
void Knapsack::dependentMutation(double pm) {
  mutate_binary(pm);
  repair();
}

// Random generation of an individual
void Knapsack::restart(void) {
  for (int i = 0; i < nItems; i++) {
    setVar(i, ((double)rand() / (double)RAND_MAX));
    setVar(i, getVar(i) * (getMaximum(i) - getMinimum(i)) + getMinimum(i));
    if (getVar(i) >= 0.5)
      setVar(i, 1.0);
    else
      setVar(i, 0.0);
  }
  repair();
}

// Repairs an individual with capacity restriction
void Knapsack::repair(void) {
  int i = 0;
  while (!checkCapacity()) {
    setVar(deleteOrder[i], 0.0);
    i++;
  }
}

// Checking if the packed weights is over the capacity
bool Knapsack::checkCapacity() {
  float packed = 0;
  for (int i = 0; i < nItems; i++) {
    packed += getWeight(i) * int(getVar(i));
  }
  return (packed > capacity) ? false : true;
}

// Evaluation of an individual
void Knapsack::evaluate(void) {
  float objective = 0;
  for (int j = 0; j < nItems; j++) {
    objective += profits[j] * int(getVar(j));
  }
  if (!checkCapacity()) {
    repair();
    evaluate();
  } else
    setObj(0, objective);
}

// Calculates the max profit/weight ratio for each item in increasing order
void Knapsack::ratios(void) {
  float *maxRatio = new float[nItems];
  for (int j = 0; j < nItems; j++) {
    maxRatio[j] = -1.0;
    for (int i = 0; i < this->getNumberOfObj(); i++) {
      float ratio = getProfit(j) / getWeight(j);
      if (ratio >= maxRatio[j]) maxRatio[j] = ratio;
    }
  }
  quicksort(deleteOrder, maxRatio, 0, nItems - 1);
  delete maxRatio;
}

// Quicksort algorithm to order the max profit/weight ratio vector
void Knapsack::quicksort(vector<int> &deleteOrder, float *maxRatio, int begin,
                         int end) {
  float pivote = maxRatio[end];
  int i = begin;
  int j = end;
  bool cont = true;

  if (begin >= end) return;

  while (cont) {
    while (maxRatio[++i] < pivote)
      ;
    while (maxRatio[--j] > pivote)
      ;
    if (i < j) {
      float temp = maxRatio[i];
      int aux = deleteOrder[i];
      maxRatio[i] = maxRatio[j];
      deleteOrder[i] = deleteOrder[j];
      maxRatio[j] = temp;
      deleteOrder[j] = aux;
    } else
      cont = false;
  }

  float temp = maxRatio[i];
  int aux = deleteOrder[i];
  maxRatio[i] = maxRatio[end];
  deleteOrder[i] = deleteOrder[end];
  maxRatio[end] = temp;
  deleteOrder[end] = aux;

  quicksort(deleteOrder, maxRatio, begin, i - 1);
  quicksort(deleteOrder, maxRatio, i + 1, end);
}

Individual *Knapsack::clone(void) const { return new Knapsack(); }
