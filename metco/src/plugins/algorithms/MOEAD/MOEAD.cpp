/***********************************************************************************
 * AUTHORS
 *   - Alejandro Marrero
 *   - Eduardo Segredo
 *
 * DATE
 *   November 2018
 * *********************************************************************************/

#include "MOEAD.h"

#include <algorithm>
#include <limits>

// Constantes para la gestion del threshold en las restricciones
const int MOEAD::LINEAR_THRESHOLD = 0;
const int MOEAD::ADAPTATIVE_THRESHOLD = 1;
const int MOEAD::FIXED_THRESHOLD = 2;
const double MOEAD::INITIAL_LINEAR_THRESHOLD = 0.01;
const int MOEAD::NUM_PARAMS = 5;

// Constructor
MOEAD::MOEAD() { secondPopulation = new vector<Individual *>; }

// Destructor
MOEAD::~MOEAD(void) {
  for (int i = 0; i < secondPopulation->size(); i++)
    delete ((*secondPopulation)[i]);
  delete (secondPopulation);

  for (int i = 0; i < neighbourhood.size(); i++) {
    neighbourhood[i].clear();
  }
  neighbourhood.clear();

  for (int i = 0; i < weightVector.size(); i++) {
    weightVector[i].clear();
  }
  weightVector.clear();

  referencePoint.clear();
  violationDegrees.clear();
}

void MOEAD::runGeneration() {
  // For each individual (subproblem) in the current population
  // computePenalties();
  std::cout << "Ejecutando generacion: " << this->getGeneration() << std::endl;
  std::cout << "Evaluaciones: " << this->getPerformedEvaluations() << std::endl;
  for (int i = 0; i < getPopulationSize(); i++) {
    // Creates a new offspring by applying the variation operators
    // LA EVALUACION SE REALIZA TRAS LA CREACION
    Individual *offSpring = createOffspring(i);
    offSpring->evaluate();
    // Updates the state of the algorithm
    updateReferencePoint(offSpring);
    updateNeighbouringSolution(offSpring, i);
    updateSecondPopulation(offSpring);
    delete (offSpring);
  }
}

/**
 * Metodo que calcula las penalizaciones de los individuos de la poblacion
 * Define los valores máximos y mínimos y el threshold
 *
 **/
void MOEAD::computePenalties() {
  for (int i = 0; i < getPopulationSize(); i++) {
    violationDegrees[i] = (*population)[i]->computeFeasibility();
  }

  if (thresholdPolicy == ADAPTATIVE_THRESHOLD) {
    minConstViolation =
        *std::min_element(violationDegrees.begin(), violationDegrees.end());
    maxConstViolation =
        *std::max_element(violationDegrees.begin(), violationDegrees.end());
    vioThreshold =
        minConstViolation + 0.3 * (maxConstViolation - minConstViolation);
  } else if (thresholdPolicy == LINEAR_THRESHOLD) {
    vioThreshold = vioThreshold * getGeneration();
  }
}

bool MOEAD::init(const vector<string> &params) {
  // Check for the algorithm parameters
  if (params.size() < NUM_PARAMS) {
    cerr << "Error MOEA/D: incorrect parameters" << endl;
    cerr << "Number of subproblems N" << endl;
    cerr << "Neighbourhood size T" << endl;
    cerr << "Weight vectors file name" << endl;
    cerr << "Mutation rate pm" << endl;
    cerr << "Crossover rate pc" << endl;
    /*  cerr << "Threshold policy: " << endl;
      cerr << "\t- 0 = Linear" << endl;
      cerr << "\t- 1 = Adaptative" << endl;
      cerr << "\t- 2 = Fixed + value" << endl;*/
    return false;
  }

  // Initialisation of parameters and all data structures required
  setNumberOfSubProblems(atoi(params[0].c_str()));
  setNeighbourhoodSize(atoi(params[1].c_str()));
  setFileName(params[2].c_str());
  setMutationRate(atof(params[3].c_str()));
  setCrossoverRate(atof(params[4].c_str()));
  setNumberOfObj(getSampleInd()->getNumberOfObj());
  setPopulationSize(getNumberOfSubProblems());
  initialiseReferencePoint();
  initialiseWeightVector();
  initialiseNeighbourhood();
  // Initialisation of the degree violation vector
  /*violationDegrees.resize(getPopulationSize(),
                          std::numeric_limits<double>::max());
  // Gestion de las restricciones
  thresholdPolicy = atoi(params[5].c_str());
  // Si es fijo lo definimos al inicio y no cambiamos
  if (thresholdPolicy == FIXED_THRESHOLD) {
    if (params.size() != (NUM_PARAMS + 1)) {
      return false;
    } else {
      vioThreshold = atof(params[6].c_str());
    }
  } else if (thresholdPolicy == LINEAR_THRESHOLD) {
    vioThreshold = INITIAL_LINEAR_THRESHOLD;
  }*/
  return true;
}

// Get solution from the external population (non-dominated solutions)
void MOEAD::getSolution(MOFront *p) {
  for (unsigned int i = 0; i < secondPopulation->size(); i++) {
    p->insert((*secondPopulation)[i]);
  }
}

void MOEAD::initialiseReferencePoint() {
  for (int i = 0; i < getNumberOfObj(); i++) {
    if (getSampleInd()->getInternalOptDirection(i) == MAXIMIZE) {
      referencePoint.push_back(DBL_MIN);
    } else if (getSampleInd()->getInternalOptDirection(i) == MINIMIZE) {
      referencePoint.push_back(DBL_MAX);
    }
  }

  Individual *ind = getSampleInd()->internalClone();
  ind->restart();
  ind->evaluate();
  updateReferencePoint(ind);
  delete (ind);
}

void MOEAD::initialiseWeightVector() {
  weightVector = vector<vector<double>>(getPopulationSize(),
                                        vector<double>(getNumberOfObj(), 0));

  ifstream inputFile;
  inputFile.open(fileName.c_str());
  if (!inputFile) {
    cerr << "Error MOEA/D: file containing weight vectors could not be opened"
         << endl;
    exit(1);
  }

  int numWeightVectors;
  inputFile >> numWeightVectors;

  if (numWeightVectors != getPopulationSize()) {
    cerr << "Error MOEA/D: the number of weight vectors does not match with "
            "the number of subproblems specified"
         << endl;
    inputFile.close();
    exit(1);
  }

  for (int i = 0; i < getPopulationSize(); i++) {
    for (int j = 0; j < getNumberOfObj(); j++) {
      inputFile >> weightVector[i][j];
    }
  }
  inputFile.close();
}

/**
 *  Metodo que inicializa el vecindario de cada uno de los individuos
 *  Define la relacion de vecindad segun la distancia
 **/
void MOEAD::initialiseNeighbourhood() {
  if (getNeighbourhoodSize() > getPopulationSize()) {
    cerr << "Error MOEA/D: the neighbourhood size (T) cannot be greather than "
            "the number of subproblems (N)"
         << endl;
    exit(1);
  }
  neighbourhood = vector<vector<int>>(getPopulationSize(), vector<int>());
  for (int i = 0; i < getPopulationSize(); i++) {
    vector<int> indx;
    vector<double> dist;
    for (int j = 0; j < getPopulationSize(); j++) {
      indx.push_back(j);
      double tp = getEuclideanDistance(weightVector[i], weightVector[j]);
      dist.push_back(tp);
    }
    minFastSort(dist, indx, getPopulationSize(), getNeighbourhoodSize() + 1);
    for (int k = 0; k < getNeighbourhoodSize(); k++) {
      neighbourhood[i].push_back(indx[k]);
    }
    indx.clear();
    dist.clear();
  }
}

/***
 * Método que aplica los operadores geneticos ç
 * para generar un nuevo individuo
 **/
Individual *MOEAD::createOffspring(const int &i) {
  // Selects two neighboring solutions randomly
  int id1 = (int)(getNeighbourhoodSize()) * (rand() / (RAND_MAX + 1.0));
  int id2 = (int)(getNeighbourhoodSize()) * (rand() / (RAND_MAX + 1.0));
  Individual *p1 = (*population)[neighbourhood[i][id1]]->internalClone();
  Individual *p2 = (*population)[neighbourhood[i][id2]]->internalClone();
  // Crossover
  double vcross = rand() / (RAND_MAX + 1.0);
  if (vcross < pc) {
    p1->crossover(p2);
  }
  // Mutation
  // Potential improvement: mutate p2 and select the best individual from both
  // p1 and p2
  p1->mutation(pm);
  evaluate(p1);
  // Free memory
  delete (p2);
  return p1;
}

/**
 * Método que actualiza el punto de referencia
 * si hemos encontrado nuevos optimos
 **/
void MOEAD::updateReferencePoint(Individual *ind) {
  for (int i = 0; i < getNumberOfObj(); i++) {
    if (ind->getInternalOptDirection(i) == MAXIMIZE) {
      if (ind->getObj(i) > referencePoint[i]) {
        referencePoint[i] = ind->getObj(i);
      }
    } else if (ind->getInternalOptDirection(i) == MINIMIZE) {
      if (ind->getObj(i) < referencePoint[i]) {
        referencePoint[i] = ind->getObj(i);
      }
    }
  }
}

/**
 * Metodo para actualizar la poblacion secundaria eliminando los individuos
 * dominados por el individuo que recibe como parametro
 **/
void MOEAD::updateSecondPopulation(Individual *ind) {
  unsigned int i = 0;
  // Removes from the external population all those individuals dominated by
  // individual ind
  while (i < secondPopulation->size()) {
    if (dominanceTest((*secondPopulation)[i], ind) == SECOND_DOMINATES) {
      delete ((*secondPopulation)[i]);
      (*secondPopulation)[i] =
          (*secondPopulation)[secondPopulation->size() - 1];
      secondPopulation->pop_back();
    } else
      i++;
  }
  // Adds individual ind to the external population if no individual in the said
  // population dominates it
  bool insert = true;
  i = 0;
  while (i < secondPopulation->size()) {
    if (dominanceTest((*secondPopulation)[i], ind) == FIRST_DOMINATES) {
      insert = false;
      break;
    }
    i++;
  }
  if (insert) secondPopulation->push_back(ind->internalClone());
}

/**
 *  Metodo empleado para actualizar el fitness de los vecinos de un individuo
 *
 **/
void MOEAD::updateNeighbouringSolution(Individual *offspring, const int &i) {
  for (int j = 0; j < getNeighbourhoodSize(); j++) {
    // the index of the neighbouring subproblem
    double id = neighbourhood[i][j];
    // fitness of the offspring
    double f1 = computingFitnessValue(offspring, weightVector[id]);
    // fitness of the neighbour
    double f2 = computingFitnessValue((*population)[id], weightVector[id]);

    // if the offspring is better than the neighbour, then update the neighbour
    if (f1 <= f2) {
      delete ((*population)[id]);
      (*population)[id] = offspring->internalClone();
      // Actualizamos las penalizaciones del vecino con las del offSpring
      // violationDegrees[id] = offSpringVioDegree;
    }
  }
}

/**
 * Computes the fitness value of a particular individual by considering the
 * Tchebycheff approach
 *
 * Updated to consider penalties (temporarily removed)
 **/
double MOEAD::computingFitnessValue(Individual *ind, vector<double> &lambda) {
  double fitness = DBL_MIN;
  double scaleFactor = 0.0;
  // Calculamos el factor de escalado en funcion del grado de violacion de las
  // restricciones
  /*if (violationDegree < vioThreshold) {
    scaleFactor = firstScalingParam * (violationDegree * violationDegree);
  } else {
    scaleFactor = firstScalingParam * (vioThreshold * vioThreshold) +
                  secondScalingParam * (violationDegree - vioThreshold);
  }*/
  for (int i = 0; i < getNumberOfObj(); i++) {
    double dif = 1.1 * referencePoint[i] - ind->getObj(i);
    double s = lambda[i] * (dif > 0 ? dif : -dif);
    // Al calculo de la funcion de Tchebycheff le añadimos el factor de escalado
    // s += scaleFactor;
    fitness = s;  // if (s > fitness) fitness = s;
  }
  return fitness;
}

/**
 * Metodo auxiliar que nos permite definir el orden de los individuos
 *
 **/
void MOEAD::minFastSort(vector<double> &dist, vector<int> &indx,
                        int numberSubProblems, int neighbourhoodSize) {
  for (int i = 0; i < neighbourhoodSize; i++) {
    for (int j = i + 1; j < numberSubProblems; j++) {
      if (dist[i] > dist[j]) {
        double temp = dist[i];
        dist[i] = dist[j];
        dist[j] = temp;
        int id = indx[i];
        indx[i] = indx[j];
        indx[j] = id;
      }
    }
  }
}

/**
 * Metodo que calcula la distancia Euclidea entre dos vectores de pesos
 * */
double MOEAD::getEuclideanDistance(vector<double> weightA,
                                   vector<double> weightB) {
  double dist = 0;
  for (int i = 0; i < weightA.size(); i++) {
    dist += (weightA[i] - weightB[i]) * (weightA[i] - weightB[i]);
  }
  return sqrt(dist);
}

void MOEAD::printInfo(ostream &os) const {
  os << "Multi-objective Evolutionary Algorithm based on decomposition: MOEA/D"
     << endl;
  os << "Number of Evaluations = " << getPerformedEvaluations() << endl;
  os << "Number of subproblems (N) = " << getNumberOfSubProblems() << endl;
  os << "Neighbourhood size (T) = " << getNeighbourhoodSize() << endl;
  os << "Mutation rate = " << pm << endl;
  os << "Crossover rate = " << pc << endl;
}
