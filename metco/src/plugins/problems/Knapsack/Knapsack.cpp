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
vector<item_efficiency> Knapsack::items_efficiency;

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
  computeEfficiency();
  return true;
}

// Reads a problem from file
void Knapsack::readFile(const string filename) {
  ifstream input(filename.c_str());
  if (input.fail()) {
    cerr << "Error while opening the instance file" << endl;
    exit(-1);
  }
  string readLine;
  input >> nItems;
  input >> capacity;
  profits.resize(nItems);
  weights.resize(nItems);
  setNumberOfVar(nItems);
  float weight, profit;

  for (int i = 0; i < nItems; i++) {
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

/**
 *  Metodo para reparar una solucion cuando se ha violado la restriccion
 *
 *  Recorremos el array de pares donde estan ordenados los elementos por su
 *  eficiencia y cambiamos su valor de 1 a 0.
 **/
void Knapsack::repair(void) {
  int i = 0;
  while (!checkCapacity()) {
    // Ponemos a 0 el elemento en la posicion i-esima del vector de efficiencia
    setVar(items_efficiency[i].first, 0.0);
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
void Knapsack::computeEfficiency(void) {
  items_efficiency.resize(nItems);

  for (int j = 0; j < nItems; j++) {
    items_efficiency[j].first = j;
    items_efficiency[j].second = 0.0;
    for (int i = 0; i < 1; i++) {
      items_efficiency[j].second = getProfit(j) / getWeight(j);
    }
  }
  sort(items_efficiency.begin(), items_efficiency.end(), sortByEfficiencyDesc);
}

/**
 *  Funcion auxiliar para ordenar los elementos por su eficiencia
 *
 **/
bool sortByEfficiencyDesc(const pair<int, int> &firstPair,
                          const pair<int, int> &secondPair) {
  return firstPair.second > secondPair.second;
}

Individual *Knapsack::clone(void) const { return new Knapsack(); }
