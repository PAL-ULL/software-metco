/***********************************************************************************
 * AUTHORS
 *   - Alejandro Marrero
 *   - Eduardo Segredo
 *
 * DATE
 *   November 2018
 *
 * DESCRIPTION
 *
 * Implementation of the Multi-objective Evolutionary Algorithm based on
 * Decomposition (MOEA/D) given in: Qingfu Zhang, Hui Li (2007). "MOEA/D: A
 * Multiobjective Evolutionary Algorithm Based on Decomposition". IEEE
 * Transactions on Evolutionary Computation.
 *
 * **********************************************************************************/

#ifndef __MOEAD_H__
#define __MOEAD_H__

#include <float.h>
#include <math.h>

#include <iostream>
#include <string>
#include <vector>

#include "EA.h"
#include "Individual.h"
#include "MOFront.h"

//#define __MOEAD_DEBUG__

using namespace std;

class MOEAD : public EA {
 public:
  // Constructor
  MOEAD();

  // Destructor
  virtual ~MOEAD(void);

  // Describes one iteration of the algorithm
  void runGeneration();

  // Initialises all data structures and parameters required by the approach
  bool init(const vector<string> &params);

  // Front with the current non-dominated solutions (external population or EP
  // in the corresponding paper)
  void getSolution(MOFront *p);

  // Shows information of the algorithm
  void printInfo(ostream &os) const;

  // Getters and setters
  inline string getName() const { return "MOEA/D"; }
  inline int getNumberOfSubProblems() const { return numberSubProblems; }
  inline int getNeighbourhoodSize() const { return neighbourhoodSize; }
  inline int getNumberOfObj() const { return numberOfObj; }
  inline string getFileName() const { return fileName; }
  inline double getCrossoverRate() const { return pc; }
  inline double getMutationRate() const { return pm; }

  inline void setNumberOfSubProblems(const int &numberSubProblems) {
    this->numberSubProblems = numberSubProblems;
  }
  inline void setNeighbourhoodSize(const int &neighbourhoodSize) {
    this->neighbourhoodSize = neighbourhoodSize;
  }
  inline void setNumberOfObj(const int &numberOfObj) {
    this->numberOfObj = numberOfObj;
  }
  inline void setFileName(const string &fileName) { this->fileName = fileName; }
  inline void setCrossoverRate(const double &pc) { this->pc = pc; }
  inline void setMutationRate(const double &pm) { this->pm = pm; }

 private:
  // Private attributes:

  // Crossover and mutation rates
  double pc, pm;

  // Number of subproblems, neighbourhood size, number of objective functions
  unsigned int numberSubProblems;
  unsigned int neighbourhoodSize = 10;
  unsigned int numberOfObj;

  // Degree of constraint violation
  double minConstViolation;
  double maxConstViolation;
  double vioThreshold;
  int thresholdPolicy;
  // Vector which stores the Degree of constraint violation for each individual
  vector<double> violationDegrees;
  // Scaling parameters for the Tchebycheff function
  double firstScalingParam;
  double secondScalingParam;

  // Weight vectors input file name
  string fileName;

  // Weight vectors
  vector<vector<double>> weightVector;

  // Neighbourhoods
  vector<vector<int>> neighbourhood;

  // Reference point
  vector<double> referencePoint;

  // External population
  vector<Individual *> *secondPopulation;

  // Private methods:

  // Initialises the reference point
  void initialiseReferencePoint();

  // Initialises the weight vectors
  void initialiseWeightVector();

  // Initialises the different neighbourhoods
  void initialiseNeighbourhood();

  // Initialises the population
  void initialisePopulation();

  // Generate an offsprings
  Individual *createOffspring(const int &i);

  // Updates the reference point
  void updateReferencePoint(Individual *ind);

  // Updates the external population with non-dominated solutions
  void updateSecondPopulation(Individual *ind);

  // Compares a novel offspring to its neighboring solutions in order to update
  // the neighbourhood
  void updateNeighbouringSolution(Individual *ind, const int &i);

  // Computes the fitness value of a particular individual by considering the
  // Tchebycheff approach
  double computingFitnessValue(Individual *ind, vector<double> &lambda);
  // Computes the penalties for each individual of the population
  void computePenalties();

  // Sorts neighbour weight vectors in terms of the Euclidean distance
  // between each of them and a particular weight vector in ascending order
  void minFastSort(vector<double> &dist, vector<int> &indx,
                   int numberSubProblems, int neighbourhoodSize);

  // Calculates the Euclidean distance between two weight vectors
  double getEuclideanDistance(vector<double> weightA, vector<double> weightB);

 private:
  const static int LINEAR_THRESHOLD;
  const static int ADAPTATIVE_THRESHOLD;
  const static int FIXED_THRESHOLD;
  const static double INITIAL_LINEAR_THRESHOLD;
  const static int NUM_PARAMS;
};

#endif
