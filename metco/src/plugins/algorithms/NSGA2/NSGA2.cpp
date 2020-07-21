/***********************************************************************************
 * AUTHORS
 *   - Ofelia Gonz�lez P�rez
 *   - Gara Miranda Valladares
 *   - Carlos Segura Gonz�lez
 *
 * DATE
 *   November 2007
 * *********************************************************************************/

#include "NSGA2.h"

#include <float.h>

// Define una generaci�n de b�squeda del algoritmo
void NSGA2::runGeneration() {
  // Evaluate the initial population
  if (getGeneration() == 0) {
    rankPopulation();
  }
  // Push the children at the end of the population
  createChildPop();
  // Calculate the fitness as the rank of each element
  rankCrowdingPopulation();
}

// Inicializa los par�metros iniciales del algoritmo
bool NSGA2::init(const vector<string> &params) {
  // Check for the algorithm parameters
  if (params.size() != 3) {
    cerr << "Error NSGA-II: Incorrect parameters (pm pc pSize)" << endl;
    return false;
  }

  // Initiate the parameters
  this->pm = atof(params[0].c_str());
  this->pc = atof(params[1].c_str());
  setPopulationSize(atoi(params[2].c_str()));
  return true;
}

// Rellena un frente con las soluciones actuales
void NSGA2::getSolution(MOFront *p) {
  for (unsigned int i = 0; i < population->size(); i++) {
    p->insert((*population)[i]);
  }
}

// Muestra la informaci�n relativa al algoritmo
void NSGA2::printInfo(ostream &os) const {
  os << "Genetic Algorithm NSGA2" << endl;
  os << "Number of Evaluations = " << getEvaluations() << endl;
  os << "Mutation Probability = " << pm << endl;
  os << "Crossover Probability = " << pc << endl;
  os << "Population Size = " << getPopulationSize() << endl;
}

/*******************************************************************************
 * Los siguientes m�todos son espec�ficos del NSGA-II y describen las
 * operaciones internas y el propio funcionamiento del algoritmo.
 * *****************************************************************************/

// NSGA-II crowding operator

/*************************************************************************
 * Create fronts using the non-domination rank until pSize elements have
 * been selected.
 * Each front is ordered using the local crowding distance.
 * In: population without any order with size = pSize * 2
 * Exit: population ordered using the crowded comparison operator with size
 * pSize
 * ***********************************************************************/
void NSGA2::rankCrowdingPopulation() {
  int inserted = 0;

  vector<vector<Individual *> > fronts;
  vector<Individual *> notClassified;

  // Calculate fronts
  int differentsInFront0;
  FastNondominatedSort(population, fronts, notClassified, differentsInFront0,
                       getPopulationSize());

  // Order each front using the local crowding distance
  for (int i = 0; i < fronts.size(); i++) crowOrder(fronts[i]);

  // Create the ordered population
  population->clear();
  for (int i = 0; i < fronts.size() - 1; i++) {
    for (int j = 0; j < fronts[i].size(); j++) {
      population->push_back(fronts[i][j]);
    }
  }

  // Last front
  for (int j = 0; j < fronts[fronts.size() - 1].size(); j++) {
    if (population->size() < (unsigned int)getPopulationSize()) {
      population->push_back((fronts[fronts.size() - 1][j]));
    } else {
      delete (fronts[fronts.size() - 1][j]);
    }
  }

  // Delete not used individuals
  for (int i = 0; i < notClassified.size(); i++) {
    delete (notClassified[i]);
  }
}

// Sort and rank the population
// Population has size N, it reorders the population based on the
void NSGA2::rankPopulation() {
  vector<vector<Individual *> > fronts;
  vector<Individual *> notClassified;
  int differentsInFront0 = 0;
  FastNondominatedSort(population, fronts, notClassified, differentsInFront0,
                       population->size());
  population->clear();
  for (int i = 0; i < fronts.size(); i++) {
    for (int j = 0; j < fronts[i].size(); j++) {
      population->push_back(fronts[i][j]);
    }
  }
}

/*******************************************************************************
 * Create a child population by applying:
 *  - Selection
 *  - Crossover
 *  - Recombination
 * *****************************************************************************/
void NSGA2::createChildPop() {
  int tamPop = population->size();
  vector<int> a1;

  // Se genera un vector con los numeros de 0 a tamPop-1 (2 veces)
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < tamPop; j++) {
      a1.push_back(j);
    }
  }

  // Se reordena la primera mitad
  for (int i = 0; i < tamPop; i++) {
    int rand_value =
        i + (int)(((double)(tamPop - i)) * rand() / (RAND_MAX + 1.0));
    int tmp = a1[rand_value];
    a1[rand_value] = a1[i];
    a1[i] = tmp;
  }

  // Reodenamos la segunda mitad
  for (int i = tamPop; i < tamPop * 2; i++) {
    int rand_value =
        i + (int)(((double)(2 * tamPop - i)) * rand() / (RAND_MAX + 1.0));
    int tmp = a1[rand_value];
    a1[rand_value] = a1[i];
    a1[i] = tmp;
  }

  for (int i = 0; i < (tamPop * 2) - ((tamPop * 2) % 4); i += 4) {
    // Selection
    int ind1 = a1[i];
    int ind2 = a1[i + 1];
    // Binary tournment
    if (ind1 < ind2) {
      population->push_back((*population)[ind1]->internalClone());
    } else {
      population->push_back((*population)[ind2]->internalClone());
    }
    int ind3 = a1[i + 2];
    int ind4 = a1[i + 3];
    if (ind3 < ind4) {
      population->push_back((*population)[ind3]->internalClone());
    } else {
      population->push_back((*population)[ind4]->internalClone());
    }

    // Crossover
    double vcross = rand() / (RAND_MAX + 1.0);
    if (vcross < pc) {
      (*population)[population->size() - 2]->crossover(
          (*population)[population->size() - 1]);
    }

    // Mutation
    (*population)[population->size() - 2]->mutation(pm);
    (*population)[population->size() - 1]->mutation(pm);
    evaluate((*population)[population->size() - 2]);
    insertInArchive((*population)[population->size() - 2]);
    evaluate((*population)[population->size() - 1]);
    insertInArchive((*population)[population->size() - 1]);
  }

  // If population is odd one more must be inserted
  if (tamPop % 2) {
    int ind1 = a1[a1.size() - 4];
    int ind2 = a1[a1.size() - 3];
    Individual *tmp;

    // Binary tournment
    if (ind1 < ind2) {
      tmp = (*population)[ind1]->internalClone();
    } else {
      tmp = (*population)[ind2]->internalClone();
    }
    int ind3 = a1[a1.size() - 2];
    int ind4 = a1[a1.size() - 1];
    if (ind3 < ind4) {
      population->push_back((*population)[ind3]->internalClone());
    } else {
      population->push_back((*population)[ind4]->internalClone());
    }

    // Crossover
    double vcross = rand() / (RAND_MAX + 1.0);
    if (vcross < pc) {
      (*population)[population->size() - 1]->crossover(tmp);
    }
    delete (tmp);

    // Mutation
    (*population)[population->size() - 1]->mutation(pm);
    evaluate((*population)[population->size() - 1]);
    insertInArchive((*population)[population->size() - 1]);
  }
}
