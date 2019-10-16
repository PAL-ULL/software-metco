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

#ifndef __BGWO_H__
#define __BGWO_H__

#include <iostream>
#include <memory>
#include <vector>
#include "EA.h"
#include "Individual.h"

using namespace std;

class BGWO : public EA {
 public:
  bool init(const vector<string>& parameters);
  void getSolution(MOFront* paretoFront);
  void printInfo(ostream& os) const;

 private:
  void runGeneration();
  void estimatePreyLocation();
  void computeWolvesWeights();
  void generateTrialSolution(unique_ptr<Individual>& trialSolution,
                             const int wolfIndex);

 private:
  unique_ptr<Individual> wolfAlpha;
  unique_ptr<Individual> wolfBeta;
  unique_ptr<Individual> wolfDelta;
  unique_ptr<Individual> preyLocation;

  double wolfAlphaScore;
  double wolfBetaScore;
  double wolfDeltaScore;

  double wolfAlphaWeight;
  double wolfBetaWeight;
  double wolfDeltaWeight;

  int packSize;
  double seed;

 private:
  static const int NUM_ARGS;
  static const int DEFAULT_NUM_WOLVES;
  static const int LEADER_WOLVES;
  static const int DEFAULT_SEED;
};

bool sortByObjective(const Individual* firstInd, const Individual* secondInd);
double sigmoidFunction(const double position);

#endif