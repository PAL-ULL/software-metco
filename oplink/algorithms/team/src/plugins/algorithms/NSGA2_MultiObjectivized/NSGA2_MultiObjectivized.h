/***********************************************************************************
 * AUTHORS
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * DATE
 *   November 2007
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
 * **********************************************************************************/


#ifndef __NSGA2_MultiObjectivized_H__
#define __NSGA2_MultiObjectivized_H__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>

#include "MOFront.h"
#include "Individual.h"
#include "EA.h"


class NSGA2_MultiObjectivized : public EA {
		  
public:
	// Constructor
	NSGA2_MultiObjectivized() {};

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
	
	string getName() const { return "NSGA2_MultiObjectivized"; }

	bool supportsMultiObjectivization() { return true; }
	
private:
	float pm; // Mutation probability
	float pc; // Crossover probability
	vector<Individual *> *offSpring;
	
	static const int POPSIZE_DEFAULT = 100;	// Population size by default

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
