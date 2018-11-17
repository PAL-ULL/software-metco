
#include "Tchebycheff.h"
#include <limits>
#include <math.h>
#include <iostream>

double Tchebycheff::evaluate(Individual* ind, Individual* reference, std::vector<double>& weights) {
    const int objs = ind->getNumberOfObj();
    double objective;
    if(direction == MINIMIZE) {
        objective = numeric_limits<double>::max();
    } else {
        objective = numeric_limits<double>::min();
    }
    for(int i = 0; i < objs; i++) {
        double fitness = fabs(ind->getObj(i) - reference->getObj(i));
        if (weights[i] == 0.0) {
            fitness *= 0.0001;
        } else {
            fitness *= weights[i];
        }
        if(direction == MINIMIZE && fitness < objective) {
            objective = fitness;
        } else if (direction == MAXIMIZE && fitness > objective) {
            objective = fitness;
        }
    }
    return objective;
}
