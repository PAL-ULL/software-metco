/***********************************************************************************
 * AUTHORS
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * DATE
 *    March 2008 
 *
 * DESCRIPTION
 * 
 *   VarPopSizeEA it is an algorithm proposed to avoid strong local optimum, while
 *   maintining the beneficials of trajectory-based algorithms when no strong local
 *   optimums are found.
 *   Initially it performs as a trajectory-based algorithm with a (1+1) selection 
 *   operator (or a population-based algorithm with small population depending on 
 *   the initial parameters with a (1+1) selection operator), and keeps on with 
 *   such behaviour while no stagnation is detected. When stagnation is detected 
 *   it tries to avoid the local optimums by using a (1,1) selection operator. 
 *   If stagnation can not be avoid it increases the population size.
 *
 *   ALGORITHM
 *     Parameters: i
 *   
 *     initialize(Population)
 *     WHILE (not stopping criterion){
 *       offspring = variation(Population)
 *       for i in 0..population_size{
 *         if (Population(i) has not improved for $softBlock$ generation)
 *           //i.e. offspring(i) worst than population(i) for $softBlock$ generations
 *           Population(i) = Offspring(i)
 *         else
 *           Population(i) = best(Population(i), Offspring(i)
 *       }   
 *       If ((Population blocked for $HardBloq$ generations) && (Population.size() < $MaxPopSize$){
 *         //i.e. the individual in each position i has not improved the best 
 *         //previosly found individual in position i
 *         increase population size
 *       }
 *     }
 *
 *     The variation is performed in the following way:
 *     - First, each individual i generates an offspring i' with mutation
 *     - Then, random pairs (i1', i2') of offsprings are selected and crossover applied 
 *     to them, generating (i1'', i2''). i1'' replaces i1' if it is better than it.
 *     Similarly i2'' replaces i2' if it is better than it.
 *     
 **********************************************************************************/


#ifndef MUTATIONEA_H
#define MUTATIONEA_H

#include "Individual.h"
#include "EA.h"

#include <vector>

using namespace std;


class VarPopSizeEA : public EA {

	public:
		bool init(const vector<string> &params);
		void runGeneration();
		void fillPopWithNewIndsAndEvaluate(); 
		void getSolution(MOFront *p) ;
		double *getRestartInfo();
		void setRestartInfo(double *);
		
	private:
		void internalEvaluate(Individual *ind);
		bool isBetter(double a, double b);
		void evaluation();
		void createOffspring();
		void crossoverVariation();
		void checkOffspringImprovements();
		void selection();
		void checkGlobalBlock();
	
		vector<Individual*> offSpring;
		unsigned int softBlock;
		int hardBlock, optD, initSize;
		double pm, pc;
		unsigned int maxPopSize;
		vector<unsigned int> block;
		int globalBlock;
		vector<double> bestScoreIndividual;
		void printInfo(ostream &os) const;
		bool firstGeneration;
		
};

#endif
