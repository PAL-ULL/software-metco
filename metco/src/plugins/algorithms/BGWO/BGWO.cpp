/*
 *
 * author: Alejandro Marrero
 * contact: alejandro.marrero.10@ull.edu.es
 * Created on Wed Oct 16 2019
 *
 * Copyright (c) 2019 Universidad de La Laguna
 *
 * Summary: Implementacion del algoritmo Binary Grey Wolf Optimizer.
 * Algoritmo inspirado en la versión multi-objectivo del algoritmo
 * presentado en el articulo: Luo, K., & Zhao, Q. (2019). A binary grey wolf
 * optimizer for the multidimensional knapsack problem.
 *
 *
 */

#include "BGWO.h"
#include <random>

const int BGWO::NUM_ARGS = 2;
const int BGWO::DEFAULT_NUM_WOLVES = 50;
const int BGWO::LEADER_WOLVES = 3;
const int BGWO::DEFAULT_SEED = 13;
const int INITIAL_GENERATION = 0;

bool BGWO::init(const vector<string>& parameters) {
  if (parameters.size() != NUM_ARGS) {
    cerr << "Parameters <packSize> <seed> not found!" << endl;
    cerr << "Set by default to 50 wolves" << endl;
    cerr << "Set seed to " << DEFAULT_SEED << endl;
    packSize = DEFAULT_NUM_WOLVES;
    seed = DEFAULT_SEED;
  } else {
    packSize = atoi(parameters[0].c_str());
    seed = atof(parameters[2].c_str());
  }

  setPopulationSize(packSize);
  wolfAlpha = unique_ptr<Individual>(getSampleInd());
  wolfBeta = unique_ptr<Individual>(getSampleInd());
  wolfDelta = unique_ptr<Individual>(getSampleInd());
  preyLocation = unique_ptr<Individual>(getSampleInd());

  return true;
}

/**
 *  Método que realizar el bucle principal del algoritmo
 *  En la generacion inicial debemos identificar a los lobos
 *  alpha, beta y delta.
 **/
void BGWO::runGeneration() {
  if (getGeneration() == INITIAL_GENERATION) {
    sort(population->begin(), population->end(), sortByObjective);
    wolfAlpha.reset((*population)[0]->internalClone());
    wolfBeta.reset((*population)[1]->internalClone());
    wolfDelta.reset((*population)[2]->internalClone());
    // Nos quedamos con los valores de fitness tambien
    wolfAlphaScore = wolfAlpha->getObj(0);
    wolfBetaScore = wolfBeta->getObj(0);
    wolfDeltaScore = wolfDelta->getObj(0);
  }
  estimatePreyLocation();
  for (int k = 0; k < getPopulationSize(); k++) {
    unique_ptr<Individual> trialSolution(getSampleInd()->internalClone());

    generateTrialSolution(trialSolution, k);

    // En caso de no ser un lobo lider o mejorar el valor objetivo ->
    // actualizamos
    if (k > LEADER_WOLVES ||
        trialSolution->getObj(0) > (*population)[k]->getObj(0)) {
      (*population)[k] = trialSolution->internalClone();
    }
    // Comparamos con los lobos lideres
    // Alpha
    if (trialSolution->getObj(0) > wolfAlphaScore) {
      wolfAlphaScore = trialSolution->getObj(0);
      wolfAlpha.reset(trialSolution->internalClone());
    }
    // Beta
    if (trialSolution->getObj(0) > wolfBetaScore) {
      wolfBetaScore = trialSolution->getObj(0);
      wolfBeta.reset(trialSolution->internalClone());
    }
    // Delta
    if (trialSolution->getObj(0) > wolfDeltaScore) {
      wolfDeltaScore = trialSolution->getObj(0);
      wolfDelta.reset(trialSolution->internalClone());
    }
  }
}

/**
 *  Metodo para generar una nueva solucion de prueba.
 *  Se repita para cada uno de los lobos de la manada.
 *  Tras completar la solucion de prueba, se ejecuta
 *  el operador de reparacion para mantener las restricciones.
 **/
void BGWO::generateTrialSolution(unique_ptr<Individual>& trialSolution,
                                 const int wolfIndex) {
  default_random_engine generator;
  generator.seed(seed);
  uniform_int_distribution<int> minusTwo2TwoDist(-2, 2);
  uniform_int_distribution<int> zero2OneDist(0, 1);
  const int dimension = trialSolution->getNumberOfVar();
  for (int i = 0; i < dimension; i++) {
    double firstRand = minusTwo2TwoDist(generator);
    double secondRand = zero2OneDist(generator);
    double y = preyLocation->getVar(i) -
               firstRand * abs(preyLocation->getVar(i) -
                               (*population)[wolfIndex]->getVar(i));
    double finalPos = (secondRand < sigmoidFunction(y)) ? 1 : 0;
    trialSolution->setVar(i, finalPos);
  }
  // El paper indica que debemos ejecutar el operador de reparacion
  // antes de evaluar al individuo
  trialSolution->evaluate();
}

/**
 *  Calculamos los pesos de los tres lobos dominantes
 *  a la hora de tomar decisiones de caza
 **/
void BGWO::computeWolvesWeights() {
  double sumOfWeights = wolfAlphaScore + wolfBetaScore + wolfDeltaScore;
  wolfAlphaWeight = wolfAlphaScore / sumOfWeights;
  wolfBetaWeight = wolfBetaScore / sumOfWeights;
  wolfDeltaWeight = wolfDeltaScore / sumOfWeights;
}

/**
 *  Método que estima la posicion de la presa en funcion
 *  de la posicion de los tres primeros lobos
 **/
void BGWO::estimatePreyLocation() {
  // Primero tenemos que calcular los pesos de los lobos en la toma de decision
  computeWolvesWeights();
  const int dimension = preyLocation->getNumberOfVar();
  default_random_engine generator;
  generator.seed(seed);
  normal_distribution<double> normalDistribution(0.0, getGeneration());
  double epsilon = normalDistribution(generator);

  for (int i = 0; i < dimension; i++) {
    double preyAtI = wolfAlphaWeight * wolfAlpha->getVar(i) +
                     wolfBetaWeight * wolfBeta->getVar(i) +
                     wolfDeltaWeight * wolfDelta->getVar(i) + epsilon;
    preyLocation->setVar(i, preyAtI);
  }
}

/**
 *  Suponemos que las actualizaciones de la manada se han realizado
 *  y por lo tanto el lobo Alpha sigue siendo el mejor
 **/
void BGWO::getSolution(MOFront* paretoFront) {
  paretoFront->insert(wolfAlpha.get());
}

/**
 *  Método para imprimir la información del algoritmo en el
 *  fichero de resultados.
 **/
void BGWO::printInfo(ostream& os) const {
  os << "Binary Grey Wolf Optimizer" << endl;
  os << "Number of Evaluations = " << getEvaluations() << endl;
}

/**
 *  Funcion auxiliar que nos ayuda a ordenar la manada por
 *  el valor de la funcion objetivo
 *
 **/
bool sortByObjective(const Individual* firstInd, const Individual* secondInd) {
  return (firstInd->getInternalOptDirection(0) == MINIMIZE)
             ? (firstInd->getObj(0) < secondInd->getObj(0))
             : (firstInd->getObj(0) > secondInd->getObj(0));
}

double sigmoidFunction(const double position) {
  return position / (1 + abs(position));
}