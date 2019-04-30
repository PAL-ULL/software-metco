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

const int Knapsack::ARGS = 2;

// Problem Initialization
bool Knapsack::init(const vector<string>& params) {
    // Check for the problem parameters
    if (params.size() != ARGS) {
        cout << "Error Knapsack: Incorrect parameters " << endl;
        cout << "<instance> <type_crossover>" << endl;
        return false;
    }
    filename = params[0];
    setNumberOfObj(1);
    type_crossover = atoi(params[1].c_str());
    // Reads a problem from file
    readFile(filename);
    ratios();
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

// Genetic Operators - Uniform Crossover
void Knapsack::dependentCrossover(Individual* ind) {
    if (type_crossover == CROSSOVER_UNIFORM) {
        crossover_uniform(ind);
    } else if (type_crossover == CROSSOVER_HUX) {
        crossover_HUX(ind);
    } else {
        cout << "Error en el tipo de Crossover. type_crossover = 0 -> crossover_uniforme. type_crossover = 1 -> crossover_hux" << endl;
    }

    ((Knapsack*)ind)->repair();
    repair();
}

void Knapsack::crossover_HUX(Individual* ind) {
    int indDistance = hammingDistance(this, ind);
    vector<int> idxVector;

    //Se genera un vector de valores aleatorios para seleccionar de forma aleatoria la primera
    //mitad de los elementos
    generateRandomValues(0, indDistance - 1, indDistance, idxVector);

    for (unsigned int i = 0; i < getNumberOfVar(); i++) {
        //Se cruza si los elementos son diferentes
        if (getVar(i) != ind->getVar(i)) {
            //Se comprueba si el elemento a cruzar esta en la primera mitad de la lista aleatoria
            for (unsigned int j = 0; j < floor(idxVector.size() / 2); j++) {
                //Si esta en la lista se realiza el cruce
                if (i == idxVector[j]) {
                    double tmp = ind->getVar(i);
                    ind->setVar(i, getVar(i));
                    setVar(i, tmp);
                }
            }
        }
    }
    ((Knapsack*)ind)->repair();
    repair();
}

// Random generation of an individual
void Knapsack::restart(void) {
    for (int i = 0; i < nItems; i++) {
        setVar(i, ((double)rand() / (double)RAND_MAX));
        setVar(i, getVar(i) * (getMaximum(i) - getMinimum(i)) + getMinimum(i));
        if (getVar(i) >= 0.5)
            setVar(i, 1);
        else
            setVar(i, 0);
    }
    repair();
}

// Repairs an individual with capacity restriction
void Knapsack::repair(void) {
    int i = 0;
    while (!checkCapacity()) {
        setVar(deleteOrder[i], 0);
        i++;
    }
}

// Calculates the max profit/weight ratio for each item in increasing order
void Knapsack::ratios(void) {
    vector<float> maxRatio(nItems, -1.0);
    for (int j = 0; j < nItems; j++) {
        float ratio = getProfit(j) / getWeight(j);
        if (ratio >= maxRatio[j])
            maxRatio[j] = ratio;
    }
    quicksort(deleteOrder, maxRatio, 0, nItems - 1);
    maxRatio.shrink_to_fit();
}

// Quicksort algorithm to order the max profit/weight ratio vector
void Knapsack::quicksort(vector<int>& deleteOrder, vector<float>& maxRatio, int begin, int end) {
    float pivote = maxRatio[end];
    int i = begin;
    int j = end;
    bool cont = true;

    if (begin >= end)
        return;

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

// Checks all capacities restrictions
bool Knapsack::checkCapacity(void) {
    float sum = 0;
    for (int j = 0; j < nItems; j++)
        sum += (getWeight(j) * getVar(j));
    if (sum > capacity)
        return false;
    return true;
}

// Evaluation of an individual
void Knapsack::evaluate(void) {
    float objective = 0;
    for (int j = 0; j < nItems; j++)
        objective += profits[j] * getVar(j);
    setObj(0, objective);
}

Individual* Knapsack::clone(void) const {
    return new Knapsack();
}
