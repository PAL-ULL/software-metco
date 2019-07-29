/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo Gonzalez
 *   - Carlos Segura Gonzalez
 *
 * DATE
 *    March 2011
 *
 **********************************************************************************/

#include "MonoGA.h"
#include <math.h>
#include <signal.h>
#include <cstdlib>
#include <iostream>
#include "omp.h"

// Algorithm initialization
bool MonoGA::init(const vector<string> &params) {
  // Check number of parameters
  if (params.size() != 4) {
    cout << "Parametros: popSize pm pc survivalSelection" << endl;
    cout << "Survival Selection: " << endl;
    cout << "   0: Steady State" << endl;
    cout << "   1: Generational" << endl;
    cout << "   2: Replace worst from parent and offspring" << endl;
    return false;
  }
  // Only mono-objective optimization is supported
  if (getSampleInd()->getNumberOfObj() != 1) {
    cout << "Multi-Objective not supported" << endl;
    return false;
  }

  // Parameters initialization
  setPopulationSize(atoi(params[0].c_str()));
  this->pm = atof(params[1].c_str());
  this->pc = atof(params[2].c_str());
  this->survivalSelectionType = atof(params[3].c_str());

  return true;
}

void MonoGA::evaluateOffspring() {
  for (int i = 0; i < offSpring.size(); i++) {
    evaluate(offSpring[i]);
  }
}

// MonoGA generation
void MonoGA::runGeneration() {
  sortPopulation();  // Se ordena cada vez porque el paralelo podria
                     // desordenarlo TODO: ordenar solo 1 vez
  createOffSprings();
  evaluateOffspring();
  survivalSelection();
}

void MonoGA::sortPopulation() {
  sort(population->begin(), population->end(), sortByObj0);
}

void MonoGA::binaryTournamentSelection(
    int &p1, int &p2) {  // Supone que population esta ordenada
  int opt1 = (int)(((double)(getPopulationSize())) * rand() / (RAND_MAX + 1.0));
  int opt2 = (int)(((double)(getPopulationSize())) * rand() / (RAND_MAX + 1.0));
  p1 = min(opt1, opt2);
  opt1 = (int)(((double)(getPopulationSize())) * rand() / (RAND_MAX + 1.0));
  opt2 = (int)(((double)(getPopulationSize())) * rand() / (RAND_MAX + 1.0));
  p2 = min(opt1, opt2);
}

void MonoGA::parentSelection(int &p1, int &p2) {
  binaryTournamentSelection(p1, p2);
}

void MonoGA::createOffSpring() {  // Crear 2 hijos
  int p1, p2;
  parentSelection(p1, p2);
  Individual *of1 = (*population)[p1]->internalClone();
  Individual *of2 = (*population)[p2]->internalClone();
  double vcross = rand() / (RAND_MAX + 1.0);
  if (vcross < pc) {
    of1->crossover(of2);
  }
  of1->mutation(pm);
  of2->mutation(pm);

  offSpring.push_back(of1);
  offSpring.push_back(of2);
}

void MonoGA::createOffSprings() {
  offSpring.clear();
  if (survivalSelectionType == SURVIVAL_SELECTION_SS) {  // Crear un unico
                                                         // hijo
    createOffSpring();
    delete (offSpring[1]);
    offSpring.pop_back();
  } else if ((survivalSelectionType == SURVIVAL_SELECTION_GENERATIONAL) ||
             (survivalSelectionType ==
              SURVIVAL_SELECTION_REPLACEWORST_FROMPARENTANDOFFSPRING)) {
    if (survivalSelectionType == SURVIVAL_SELECTION_GENERATIONAL) {  // Elitism
      offSpring.push_back((*population)[0]->internalClone());
    }
    while (offSpring.size() < getPopulationSize()) {
      createOffSpring();
    }
  }
  if (offSpring.size() > getPopulationSize()) {
    delete (offSpring[offSpring.size() - 1]);
    offSpring.pop_back();
  }
}

void MonoGA::survivalSelection() {
  if (survivalSelectionType == SURVIVAL_SELECTION_SS) {
    if ((*population)[0]->getInternalOptDirection(0) == MINIMIZE) {
      if ((*population)[getPopulationSize() - 1]->getObj(0) >
          offSpring[0]->getObj(0)) {
        delete (*population)[getPopulationSize() - 1];
        (*population)[getPopulationSize() - 1] =
            offSpring[0];  // Ojo: Queda desornado
      } else {
        delete offSpring[0];
      }
    } else {  // Maximizar
      if ((*population)[getPopulationSize() - 1]->getObj(0) <
          offSpring[0]->getObj(0)) {
        delete (*population)[getPopulationSize() - 1];
        (*population)[getPopulationSize() - 1] =
            offSpring[0];  // Ojo: Queda desornado
      } else {
        delete offSpring[0];
      }
    }
  } else if (survivalSelectionType == SURVIVAL_SELECTION_GENERATIONAL) {
    for (int i = 0; i < getPopulationSize(); i++) {
      delete (*population)[i];
      (*population)[i] = offSpring[i];
    }
  } else if (survivalSelectionType ==
             SURVIVAL_SELECTION_REPLACEWORST_FROMPARENTANDOFFSPRING) {
    // Nota este replace worst no es el que esta explicado en el libro de
    // Eiben Aqui nos estamos "cargando" los peores entre padres e hijos En
    // el libro de Eiben dice que se generan nu hijos (< n) y se borran los
    // nu peores padres. Habria que hacer otro operador mas y a este
    // llamarlo RPELACEWORSTFROMPARENTSANDOFFSPRING Al otro lo podemos
    // llamar GENITOR Nota: en la pagina 27 si llama replace worst a un
    // esquema en que se eligen los mejores entre padres e hijos, pero puede
    // llevar a confusion...
    for (int i = 0; i < offSpring.size(); i++) {
      population->push_back(offSpring[i]);
    }
    sortPopulation();
    for (int i = 0; i < offSpring.size(); i++) {
      delete ((*population)[population->size() - 1]);
      population->pop_back();
    }
  }
}

// OJO: Para optimizacion multi-modal esto habr� que cambiarlo
void MonoGA::getSolution(MOFront *p) {
  sortPopulation();
  p->insert((*population)[0]);
}

void MonoGA::printInfo(ostream &os) const {
  os << "MonoObjective Genetic Algorithm" << endl;
  os << "Number of Evaluations = " << getEvaluations() << endl;
  os << "Mutation Probability = " << pm << endl;
  os << "Crossover Probability = " << pc << endl;
  os << "Population Size = " << getPopulationSize() << endl;
  os << "Survival Selection Type = " << survivalSelectionType << endl;
}

bool sortByObj0(const Individual *i1, const Individual *i2) {
  return (i1->getInternalOptDirection(0) == MINIMIZE)
             ? (i1->getObj(0) < i2->getObj(0))
             : (i1->getObj(0) > i2->getObj(0));
}
