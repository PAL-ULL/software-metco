/***********************************************************************************
 *
 * AUTHORS
 *   Eduardo Manuel Segredo Gonzalez
 *   Gara Miranda Valladares
 *   Carlos Segura Gonzalez
 * 
 * DATE 
 *   December 2007
 *
 * DESCRIPTION
 *   This class inherits from EA, in order to implement the evolutionary algorithm
 *   IBEA, proposed by E. Zitzler y S. Kunzli
 *
 *   -> Eckart Zitzler and Simon Kunzli.
 *   Indicator Based Selection in Multiobjective Search.
 *   Swiss Federal Institute of Technology (ETH) Zurich.
 *
 *   This algorithm is based on the idea of using an indicator (metric), in order
 *   to help in the search process.
 *   
 ************************************************************************************/


#ifndef __IBEA_H__
#define __IBEA_H__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>

#include "Individual.h"
#include "MOFront.h"
#include "EA.h"


class IBEA : public EA {

	public:

		// Constructor
		IBEA() { valuesTable = NULL; };
	
		// Destructor
		~IBEA();

		// Defines an algorithm search generation
		void runGeneration();

		// Initializes the algorithm parameters
		bool init(const vector<string> &params);

		// Actual solution to the Pareto Front
		void getSolution(MOFront *p) ;

		// Shows the algorithm information
		void printInfo(ostream &os) const;
		string getName() const { return "IBEA"; }
		
		// Parameters Info
		virtual string getParameterName(int i) const;
		virtual string getParameterValue(int i) const;
		virtual int getNumberOfParameters() const;

	private:
		double sFactor;             // Fitness scaling factor
		float pm;                   // Mutation probability
		float pc;                   // Crossover probability
		vector<double> maxObj;      // Maximum objective values of all individuals
		vector<double> minObj;      // Minimum objective values of all individuals

		vector<Individual*> mating;	 // Mating pool

		double *valuesTable;         // Table that contains the values to
		                             // calculate the fitness
    
		// Mutators
    void setValue(const int &i, const int &j, const double &value);

		// Inspectors
		double getValue(const int &i, const int &j);

		// Selection of best individuals in the population
		void selection(void);
		
		// Selection of the mating pool (binary tournament)
		void matingSelection(void);
	
		// Evaluation of the individuals in the population
		void evaluation(void);
		
		// Mutation and crossover. From the mating pool to the new population
		void variation(void);
	
		// Calculates the fitness value of all individuals
		void fitness(void);

		// Updates the fitness value of all individuals
		void updateFitness(const int &pos);

		double getEpsilonIndicator(Individual *indA, Individual *indB);

};

#endif
