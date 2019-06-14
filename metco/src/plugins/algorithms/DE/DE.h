/*********************************************************************************** 
 * AUTHOR
 *   - María Dolores Jaraíz Simón 
 * 
 * DATE
 *   January 2008
 *
 * DESCRIPTION
 *     This class inherits from MOEA to implement the Differential Evolution algorithm 
 *   based on NSGA-II for Multi-Objection Optimization.
 *   
 * ------------------------------------------------------- 
 *     DE: Differential Evolution (Price and Storn, 1997).
 * -------------------------------------------------------
 *   
 *     In Differential Evolution (DE), every pair of vectors (Vx, Vy) defines a 
 *   vector differential: Vx - Vy. When Vx and Vy are chosen randomly, their weighted 
 *   differential is used to perturb another randomly chosen vector Vz. This process 
 *   can be mathematically shown as: 
 *   
 *	                             Vz' = Vz + F(Vx - Vy)
 *   
 *     The scaling factor (F) is a user supplied constant in the range (0 < F <=1.2). 
 *   The optimal value of F for the most of the problems is in the range 0.4 <= F <= 1.0. 
 *   In every generation, each vector, Vi, is targeted for crossover with a vector 
 *   like Vz' to produce a trial vector Vt. CR represents the crossover constant in 
 *   the range 0 <= CR <= 1. 
 *
 *     On the other hand, different strategies (Price & Storn, 2003) can be adopted 
 *   in DE algorithm. In this work, the DE/rand/1/bin strategy has been implemented.
 *   In this convention, 
 *       -  DE: stands for Differential Evolution.
 *       -  rand: means that the vector Vz (vector to be mutated) is chosen randomly.
 *       -  1: is the number of difference vectors considered for the mutation of Vz.  
 *             In this case, there is a single vector difference.
 *             For perturbation with a single vector difference, out of the three 
 *			   distinct randomly chosen vectors, the weighted vector differential of 
 *			   any two vectors is added to the third one.
 *       -  bin: indicates binomial crossover.  
 *
 *
 * -------------------------------------------------------------------------------
 *     
 *     Besides, the ranking and crowding mechanisms from NSGA-II are used to choose 
 *   the best vectors for the new population.
 *     
 * ----------------------------------------------------------------------------------
 *
 *     A continuación, se expone el algoritmo NSGA-II en el que se basa la versión de 
 *   DE para optimización multiobjetivo.
 *
 * -----------------------------------------------------------------------------------
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
 *
 * **********************************************************************************/

#ifndef __DE_H__
#define __DE_H__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>

#include "utils.h"  // He añadido
#include "MOFront.h"
#include "Individual.h"
#include "EA.h"


class DE : public EA {
		  
public:
	// Constructor
	DE() {};

	// Define una generación de búsqueda del algoritmo	
	void runGeneration();

	// Inicializa los parámetros iniciales del algoritmo
	bool init(const vector<string> &params);

	// Rellena un frente con las soluciones actuales
	void getSolution(MOFront *p) ;

	// Muestra la información relativa al algoritmo	
	void printInfo(ostream &os) const;
 
	static int objOrder;	// Variable indice para el objetivo actual


private:
	float F; 
	float CR; 
	
	bool genPareto;	// Indicates if necesary to generate a local front (sequential executions)

	static const int POPSIZE_DEFAULT = 100;	// Population size by default

	// Evaluation of the individuals in the population
	void evaluation (void);

	// Sort and rank the crowding population
	void rankCrowdingPopulation();

	// Sort and rank the population
	void rankPopulation();

	// Generate the offsprings
	void createChildPop();
	
	double check_bounds(Individual *ind, double value, int position);
};

#endif
