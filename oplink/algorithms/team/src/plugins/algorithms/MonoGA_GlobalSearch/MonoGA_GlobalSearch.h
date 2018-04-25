/***********************************************************************************
 * AUTHORS
 *  - Eduardo Segredo Gonzalez
 *  - Carlos Segura Gonzalez
 * 
 * DATE
 *    March 2011 
 *
 * DESCRIPTION
 *		Mono-Objective Genetic Algorithm with several survival selection operators
 *     
 **********************************************************************************/


#ifndef MONOGA_GLOBAL_SEARCH_H
#define MONOGA_GLOBAL_SEARCH_H

#include "Individual.h"
#include "EA.h"
#include <functional>

#include <vector>

using namespace std;

#define SURVIVAL_SELECTION_SS 0
#define SURVIVAL_SELECTION_GENERATIONAL 1
#define SURVIVAL_SELECTION_REPLACEWORST_FROMPARENTANDOFFSPRING 2

#define GLOBAL_SEARCH_SIMILARITY 0
#define GLOBAL_SEARCH_ORIGINAL 1
#define GLOBAL_SEARCH_NO 2

#define NM1 1.0
#define NM2 2.0
#define NM3 0.5
#define NM4 0.5

class MonoGA_GlobalSearch : public EA {

	public:
		bool init(const vector<string> &params);
		void getSolution(MOFront *p);
		void printInfo(ostream &os) const;
	private:
		void fillPopWithOppositionBasedLearning();
		void fillPopWithNewIndsAndEvaluate();
		void runGeneration();
		void evaluateOffspring();
		void sortPopulation();
		void binaryTournamentSelection(int &p1, int &p2);
		void parentSelection(int &p1, int &p2);
		void createOffSpring();
		void createOffSprings();
		void survivalSelection();
    
    int maxVarValues;
    double varThreshold;
    vector<double> varValues;

    // Global search selection
    void globalSearch();    

    // Original proposal
    void GlobalSearchGlobalNeighbourhood();
    // Similarity-based Global Search (SGS): Linear descending function
    void GlobalSearchGlobalNeighbourhood8();
    // Similarity-based Global Search (SGS): Linear ascending function
    void GlobalSearchGlobalNeighbourhood9();
    // Similarity-based Global Search (SGS): Considers variability of the population and a threshold value
    void GlobalSearchGlobalNeighbourhood10();
    // Similarity-based Global Search (SGS): Global Neighbourhood Path Search
    void globalNeighbourhoodPathSearch();
    // Similarity-based Global Search (SGS): After COAP reviews
    void GlobalSearchGlobalNeighbourhood12();
    
    void GlobalSearchGlobalNeighbourhood3_2();
    void GlobalSearchGlobalNeighbourhood6();
    void GlobalSearchGlobalNeighbourhood6_2();

    void NelderMeadNoShrink();
    
    bool sortByDistanceToBest(const Individual *i1, const Individual *i2, const Individual *best);
    bool sortByDistanceToBestReversed(const Individual *i1, const Individual *i2, const Individual *best);

    // Attributes required to calculate diversity metrics
    double maxMeanDCN, maxMeanPairwise;

    // Functions to calculate diversity metrics
    double getEuclideanDistance(Individual *ind1, Individual *ind2);

    double getDistanceToClosestNeighbour(Individual *ind);
    double getMeanDistanceToClosestNeighbour();
    double getNormalisedMeanDistanceToClosestNeighbour();

    double getMeanPairwiseDistanceAllIndividuals();
    double getNormalisedMeanPairwiseDistanceAllIndividuals();
		
		vector<Individual *> offSpring;
		double pm, pc;
		int survivalSelectionType;
		int globalSearchType;
		bool globalSearchRequired;
};

bool sortByObj0(const Individual *i1, const Individual *i2);

#endif
