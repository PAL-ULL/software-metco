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
 * 	This class inherits from EA, in order the implement the evolutionary algorithm
 * 	SPEA (Strength Pareto Evolutionary Algorithm) proposed by Zitzler and Thiele:
 *   -> E. Zitzler, L. Thiele
 *      An evolutionary algorithm for multiobjective optimization: 
 *      The strength Pareto Approach. 
 *      Technical Report 43, Zürich, Switzerland: Computer Engineering and Networks Laboratory (TIK),
 *      Swiss Federal Institute of Technology (ETH).
 *
 * The algorithm introduce the concept of external archive, very popular now in multi-objetive EAs.
 * It introduce elitism, maintaing an external archive P' with size N', storing non-dominated
 * individuals. In each generation, such set in updated. 
 * If |P'| > N' the set size is reduced to N', using a clustering technique, selecting in each
 * cluster the chromosome with smallest distance to the remaining chromosomes (the nearest to the
 * center).
 * fitness is calculatted as Fitness(i) = Strength(i)
 *  Strength of individual i in P: S_i = n_i / (N+1)
 *  	n_i es el number of dominated solutions by i in P
 *
 * SPEA Algorithm
 *   Step 1. Generate initial population P. Empty P'.
 *   Step 2. Copy non-dominated individuals of P to P'.
 *   Step 3. Remove from P' dominated individuals.
 *   Step 4. if |P'| > N', reduce the size of the set to N', using clustering techniques.
 *   Step 5. Calculate fitness in P and P'.
 *   Step 6. Selects N individuals from P and P' using binary tournament
 *   Step 7. Apply variation operators.
 *   Step 8. Return to step 2.
 *		 
 * Clustering Algorithm
 *   Step 1. Initially each element constitutes a cluster. if |P'| <= N' then go to Step 5
 *   Step 2. For each pair of clusters calculate the distance between them. It is calculated as
 *           teh average distance of all the elements.
 *   Step 3. Combine the 2 clusters with smallest distance.
 *   Step 4. If |P'| <= N', finallize. Otherwise, go to Step 2.
 *   Step 5. Select a solution in each cluster, the one with minimum distance to the remaining
 *           elements in the cluster.
 *
 * Fitness Assignation
 *   In P':
 *      f_i = Strength_i = n_i / (N+1), 
 *          n_i is the number of dominated solutions by i in P
 *          N = |P|
 *   In P:
 *      f_i = 1 + (Strength_j foreach j in P', which dominantes i)
 *
 ************************************************************************************/

		
#ifndef __SPEA_H__
#define __SPEA_H__

#include <iostream>
#include <vector>
#include <string>
#include <math.h>

#include "MOFront.h"
#include "Individual.h"
#include "EA.h"


class SPEA : public EA {
	
	public:
		// Constructor
		SPEA ();
		
		// Destructor
		virtual ~SPEA (void);
	
		// Define una generación de búsqueda del algoritmo 
		void runGeneration();

		// Inicializa los parámetros iniciales del algoritmo
		bool init(const vector<string> &params);

		// Rellena un frente con las soluciones actuales
		void getSolution(MOFront *p) ;

		// Muestra la información relativa al algoritmo
		void printInfo(ostream &os) const;
	
		string getName() const { return "SPEA"; }

	private:
		int aSize;        // Archive size
		float pm;         // Mutation probability
		float pc;         // Crossover probability
		
		vector<Individual*>* archive;  // Archive pool
		vector<Individual*> mating;    // Mating pool
	
		// Selection of the mating pool (binary tournament)
		void matingSelection (void);
	
		// Evaluation of the individuals in the population
		void evaluation (void);
		
		//Mutation & crossover. From the manting pool to the new population
		void variation (void);
	
		// Evaluation all element in the population
		void fitness (void);
		
		// Marks all nondominated individuals according to the population with fitness 0
		void markNondominated (void);
	
		// Remove solutions within the external set which are covered by any other set member
		void updateExternalNondominatedSet (void);
		
		// Calculate the dist between individuals
		double calcDistance (const Individual* i1, const Individual* i2);
	
		// Calcula la distancia entre dos elementos del cluster
		double clusterDistance (vector<Individual*> *nonDominatedSet, vector<int> *clusters, vector<int> *clusterList,
										int cluster1, int cluster2);
	
		// Join two clusters
		void joinClusters(vector<int> *clusters, vector<int> *clusterList, int cluster1, int  cluster2, int& numOfClusters);

		// Calculate the point with minimal average distance to all other points in the cluster
		int clusterCentroid(vector<Individual*> *nonDominatedSet, vector<int> *clusters, vector<int> *clusterList, int cluster);
		
		// Reduce the external set by means of clustering
		void clustering (vector<Individual*> *nonDominatedSet, const int maxSize);
	
};

#endif
