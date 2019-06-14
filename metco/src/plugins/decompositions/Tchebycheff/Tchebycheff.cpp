
#include "Tchebycheff.h"
#include <limits>
#include <cmath>
#include <cstdlib>

double Tchebycheff::evaluate(Individual* ind, Individual* reference, std::vector<double>& weights) {
    const int objs = ind->getNumberOfObj();
    double objective = numeric_limits<double>::min();
    double fixedW, gte;
    for(int i = 0; i < objs; i++) {
        (weights[i] == 0.) ? (fixedW = 1e-4) : (fixedW = weights[i]);
        double gte = fixedW * abs(ind->getObj(i) - reference->getObj(i));
        if(gte > objective){
            objective = gte;
        }
    }
    return objective;
}
