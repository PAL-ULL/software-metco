/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González
 *   - Carlos Segura González
 * 
 * DATE
 *   May 2013
 *
 * DESCRIPTION
 *   This class inherits from EA, in order to implement NSAG-II. NSGA-II is an 
 *   improved version of NSGA.
 *    
 *   -----------------------------------------------------
 *   NSGA: Non-dominated Sorting GA (Srinivas & Deb, 1995)
 *   -----------------------------------------------------
 *   -> N. Srinivas, K. Deb: 
 *      Multiobjetive Optimization Using Nondominated Sorting in Genetic Algorithms. 
 *      Evolutionary Computation 2 (1995) 221-248.
 *     
 *   -----------------------------------------------------
 *   NSGA-II: Non-dominated Sorting Genetic Algorithm II 
 *   -----------------------------------------------------
 *   The algorithm was proposed by Deb in 2000. It is based on:
 *   - A fast non-dominated sorting approach, based on dominance fronts
 *   - A crowding operator
 *   - A selection operator which creates a mating pool by cominning the parent and 
 *   child populations and selecting the best with respect to fitness and spread
 *
 *   This version incorporates a Fuzzy Logic Controller to adapt the value of the
 *   mutation rate p_m
 * **********************************************************************************/


#ifndef __NSGA2_MultiObjectivized_Fuzzy_H__
#define __NSGA2_MultiObjectivized_Fuzzy_H__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>

#include "MOFront.h"
#include "Individual.h"
#include "EA.h"
#include "fl/Headers.h"


class NSGA2_MultiObjectivized_Fuzzy : public EA {
		  
public:
	// Constructor
	NSGA2_MultiObjectivized_Fuzzy() {};

	// Define una generación de búsqueda del algoritmo	
	void runGeneration();

	// Inicializa los parámetros iniciales del algoritmo
	bool init(const vector<string> &params);

	// Rellena un frente con las soluciones actuales
	void getSolution(MOFront *p) ;
	
	// Llamado despues de que se hagan reemplazamientos en las islas
	void exchangePerformed();

	// Muestra la información relativa al algoritmo	
	void printInfo(ostream &os) const;
	
	string getName() const { return "NSGA2_MultiObjectivized_Fuzzy"; }

	bool supportsMultiObjectivization() { return true; }
	
private:
  // Fuzzy Engine
  fl::Engine* fuzzyEngine;
  // Fuzzy Input Variables
  fl::InputVariable* fit;
  fl::InputVariable* pmIn;
  fl::InputVariable* var;
  // Fuzzy Output Variables
  fl::OutputVariable* pmOut;

  long double delta;
  int numGenToFuzzify;
  int k;

  bool initStage;
  long double prevBestFitness;

  vector<long double> initStageValues;
  vector< vector<long double> > improvements;
  vector< vector<long double> > memberships;
  vector<long double> weightedImprovements;
  vector<long double> pmHistory; 
  vector<long double> improvementHistory;
  vector<long double> momentHistory;

	long double pm; // Mutation probability
	long double pc; // Crossover probability
  
	vector<Individual *> *offSpring;
	
	static const int POPSIZE_DEFAULT = 100;	// Population size by default

  // Looks for the best individual between the parents and the offsprings
  Individual* lookForBestIndividual();

  // Methods to obtain the values to fuzzify the variables THR-IN and VAR
  long double getFitValue();
  long double getVarValue();

	// Evaluation of the individuals in the population
	void evaluation (void);

	//Calculate the fictitious objectives
	void multiObjectivize(vector<Individual*> *update);

	// Sort and rank the crowding population
	void rankCrowdingPopulation();

	// Sort and rank the population
	void rankPopulation();

	// Generate the offsprings
	void createChildPop();

	void fillExternalArchive();
	
};


#endif
