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
 *   This class inherits from EA, in order to implement the evolutionary algorithm
 *   SPEA2. SPEA2 is an improved version of SPEA, proposed by Zitzler in 2001:
 *   -> Eckart Zitzler, Marco Laumanns, Lothar Thiele
 *      SPEA2: Improving the Strength Pareto Evolutionary Algorithm.
 *      Zürich, TIK Report Nr. 103, Computer Engineering and Networks Lab (TIK), 
 *      Swiss Federal Institute of Technology (ETH) Zurich, May, 2001.
 *
 *   SPEA2 has 3 main differences with SPEA:
 *   1.- It incorporates a fine-grained fitness assignment strategy, which considers
 *       for each candidate solution the number of individuals that are dominated by 
 *       it, and the number of individuals which it dominates.
 *   2.- It incorporates a density estimation technique
 *   3.- It has an enhanced archive truncation method.
 *
 *
 *   SPEA2 Algorithm
 *     While(not stopping criterion){
 *       Step 1. Initialization: Generate an initial population P_0 and create an
 *  	           empty archive P'_0
 *       Step 2. Fitness assignment: Calculate fitness values of individuals in P_t and P'_t
 *       Step 3. Environmental Selection: Copy nondominated individuals in P_t and P'_t to 
 *               P'_(t+1). If |P'_(t+1)| > N, reduce P'_(t+1) with the truncation operator, 
 *               otherwise fill P'_(t+1) with dominated individuals in P_t and P'_t 
 *               (copy the first N' - |P'_(t+1)| individuals i with F(i) >= 1)
 *       Step 4. Mating Selection: perform binary tournament selection on P'_(t+1) in order 
 *               to fill the mating pool
 *       Step 5. Variation: apply crossover and mutation operators to the mating pool and 
 *               set P_(t+1) to the resulting population.
 *
 *   Truncation operator
 *     It iteratively removes individuals from P'_(t+1) until |P'_(t+1)|=N'. 
 *     At each iteration the individual i which i <=d j for all j in P'_(t+1) is 
 *     selected with:
 *        i <=d j: <==> forall 0 < k < |P'_(t+1)|: dist(i, k) = dist(j, k) or
 *                      exist  0 < k < |P'_(t+1)|: [(forall 0 < l < k: dist(i, l) = dist(j, l)) and (dist(i, k) < dist(j, k))]
 *
 *        where dist(i, k) denotes the distance of i to its k-th nearest neighbor in |P'_(t+1)|.
 *        
 *        That is, the individual which has the minimum distance to another individual is chosen 
 *        at each stage; if there are several individuals with minimum distance the tie is
 *        broken by considering the second smallest distances and so forth.
 *   
 *      
 *   Fitness Assignment
 *       Each individual i in P'_t and P_t is assigned a strength value S(i), 
 *       representing the number of solutions it dominates:
 *          S(i) = |{j / j in (P_t + P'_t) and i > j}|
 *
 *       The raw fitness R(i) of an individual i is calculated:
 *
 *         R(i) = sum (for j in (P_t + P'_t), j>i) S(j), 
 *            that is the sum of the strenght of the individuals that dominates i.
 *      
 *       The fitness must be minimized.
 *
 *       Additional density information is incorporated to discriminate between individuals having
 *       identifical raw fitness values. For each individual i the distances (in objective space) to
 *       all individuals in archive and population are calculated and stored in a list. The list is
 *       ordered in increasing order, the k-th element gives the distance sought, denoted as dist(i,k).
 *       k = sqrt(N + N'). The density is calculated as:
 *          D(i) = 1 / (dist(i, k) + 2)
 *
 *       The fitness is calculated as:
 *          F(i) = R(i) + D(i)
 ************************************************************************************/


#ifndef __SPEA2_H__
#define __SPEA2_H__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>

#include "Individual.h"
#include "MOFront.h"
#include "EA.h"


class SPEA2 : public EA {

	public:
		// Constructor
		SPEA2 ();
	
		// Destructor
		virtual ~SPEA2 (void);

		// Define una generación de búsqueda del algoritmo
		void runGeneration();

		// Inicializa los parámetros iniciales del algoritmo
		bool init(const vector<string> &params);

		// Rellena un frente con las soluciones actuales
		void getSolution(MOFront *p) ;

		// Muestra la información relativa al algoritmo
		void printInfo( ostream &os) const;
		
		string getName() const { return "SPEA2"; }
		
		//Parameters info
		virtual string getParameterName(int i) const;
		virtual string getParameterValue(int i) const;
		virtual int getNumberOfParameters() const;

	private:
		unsigned int aSize;  // Archive size
		int k;               // k = sqrt(N + N_)
		float pm;            // Mutation probability
		float pc;            // Crossover probability
	
		vector<Individual*>* archive;  // Archive pool
		vector<Individual*> mating;	 // Mating pool
	
		// Selection of best individuals in the population
		void selection (void);
		
		// Selection of the mating pool (binary tournament)
		void matingSelection (void);
	
		// Evaluation of the individuals in the population
		void evaluation (void);
		
		// Mutation and crossover. From the mating pool to the new population
		void variation (void);
	
		// Evaluation all element in the population
		void fitness (void);

		// Calculate distance between two individuals
		double calcDistance (const Individual* ind1, const Individual* ind2);

		// Truncate operator	
		void truncate (vector<Individual*> *v, const int size);


};

#endif
