/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo Gonzalez
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#include "Crossover_OPC.h"

#include <math.h>

void Crossover_OPC::crossover (Individual *ind1, Individual *ind2) {

  int nvars = ind1->getNumberOfVar();
  int vcross = int((double)(rand()) / (RAND_MAX + 1.0) * nvars);

  if (vcross >= (nvars / 2)) {
    // Union del primer trozo del individuo 1 con el segundo trozo
    // individuo 2
    for (unsigned int i = (vcross + 1); i < nvars; i++) {
      double tmp = ind2->getVar(i);
      ind2->setVar(i, ind1->getVar(i));
      ind1->setVar(i, tmp);
    }
  }
  else {
    // Union del segundo trozo del individuo 1 con el primer trozo
    // individuo 2
    for (unsigned int i = 0; i < vcross; i++) {
      double tmp = ind2->getVar(i);
      ind2->setVar(i, ind1->getVar(i));
      ind1->setVar(i, tmp);
    }
  }
}
