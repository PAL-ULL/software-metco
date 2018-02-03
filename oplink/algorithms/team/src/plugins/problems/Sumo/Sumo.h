/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo González
 * 
 * FECHA
 *    Octubre 2015
 * ********************************************************************************/

#ifndef __SUMO__
#define __SUMO__

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <math.h>
#include <sys/wait.h>
#include "cInstance.h"
#include "simpleXMLParser.h"

#include "Individual.h"

class Sumo : public Individual {
public:

	// Initialization method
	bool init (const vector<string> &params);

	// Evaluation
	void evaluate (void);

	// Ranges
	double inline getMaximum(const int i) const { return 60; }
	double inline getMinimum(const int i) const { return 5; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

	// Clone
	Individual* clone (void) const;

  // Print
  void print(ostream &os) const;

private:

	// Constants
	static const int NOBJ  = 1;

  // Contains the problem instance
  cInstance instance;

  // Contain the command to be executed, the tl file name and the results file name
  string command, tlFile, resultsFile;

  // Contain the mutation and crossover operators to be used
  string mut_op, cross_op;

  // Executes the simulator
  string execCommandPipe(string command);

  // Dependent mutation and crossover operators
  void dependentMutation(double pm);
  void dependentCrossover(Individual* ind);

  // Random generation of an individual
  void restart(void);
  
  // This method repairs solutions after initialization, mutation and/or crossover
  void repair(void);

  // Transforms a solution encoded by floating point numbers to a solution represented by integer ones
  void doubleToIntSolution(void);

  // This method checks if a phase only contains "y"
  bool areAllYellow(string phase);

  // Auxiliary functions
  void writeSolutionFile();
  double readFitnessFile();
};

#endif
