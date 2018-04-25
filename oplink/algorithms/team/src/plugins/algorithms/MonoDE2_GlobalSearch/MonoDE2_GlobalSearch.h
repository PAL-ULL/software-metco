/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 01/11/13
* Descripcion: Differential Evolution sin la parte de HMR que tiene MonoDE
* con el fin de que el codigo sea mas simple
************************************************************************/

#ifndef _MONODE2GLOBALSEARCH_H_
#define _MONODE2GLOBALSEARCH_H_

#include <vector>
#include <list>
#include <map>
#include <random>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "Individual.h"
#include "EA.h"
#include <functional>

using namespace std;

// Initialization Types
// Variable NelderMead
#define NM1 1.0
#define NM2 2.0
#define NM3 0.5
#define NM4 0.5
#define NMSC 1000
#define ERR_THR 1.0e-10
#define EVAL_THR 0.4

// Variable globalSearchType
#define GS_NO 1
#define GS_GLOBAL_NEIGHBOURHOOD 2
#define GS_GLOBAL_NEIGHBOURHOOD_PATH_SEARCH 3

// Variable nelderMeadType
#define NM_NO 1
#define NM_NELDER_MEAD 2
#define NM_NELDER_MEAD_ADAPTIVE 3

// Variable jumpingType
#define J_NO 1
#define J_QOBL 2
#define J_QROBL 3

// Variable popInitType
#define POP_INIT_RESTART 1
#define POP_INIT_MERSENNE_TWISTER 2
#define POP_INIT_TENT_MAP 3
#define POP_INIT_SOBOL 4
#define POP_INIT_LATTICE 5
#define POP_INIT_OPPOSITION_BASED_LEARNING 6
#define POP_INIT_QUASI_OPPOSITION_BASED_LEARNING 7
#define POP_INIT_QUASI_REFLECTED_OPPOSITION_BASED_LEARNING 8

//How to manage out-of-bounds values
//Variable reinitializationType
#define REINIT_RANDOM 1
#define REINIT_BOUNCE_BACK 2

//F Types
//Variable fType
#define F_CAUCHY_NUF 1
#define F_NORMAL 2
#define F_FIXED 3
#define F_JDE 4
#define F_COMPETITIVE 5

//CR Types
//Variable crType
#define CR_NORMAL_NUCR 1
#define CR_NORMAL 2
#define CR_CAUCHY 3
#define CR_FIXED 4
#define CR_JDE 5
#define CR_UNI_0_015_085_1 6
#define CR_COMPETITIVE 7
#define CR_UNIFORM 8

//Update NuCR strategies for NORMAL_NUCR
#define UPDATE_NUCR_JADEAMEAN 1
#define UPDATE_NUCR_NO 2
#define UPDATE_NUCR_SADE 3
#define UPDATE_NUCR_RJADE 4

//Trial generation strategies
//Variable trialStrategy
#define TRIAL_CURRENT_TO_PBEST 1
#define TRIAL_RAND_1 2
#define TRIAL_CURRENT_TO_PBEST_DIV 3
#define TRIAL_RAND_1_DIV 4
#define TRIAL_CURRENT_TO_RAND 5
#define TRIAL_CURRENT_TO_SIM 6

//Crossover strategy
//Variable crossover
#define CROSSOVER_BIN 1
#define CROSSOVER_EXP 2
#define CROSSOVER_COMBINED 3

//Update NuF strategies for Cauchy_NUF
//Variable updateNuFStrategy 
#define UPDATE_NUF_JADE 1
#define UPDATE_NUF_NO   2
#define UPDATE_NUF_INCR 3
#define UPDATE_NUF_JADEAMEAN 4

// Declaration of an extended individual
struct extIndType;

class MonoDE2_GlobalSearch : public EA {
	public:
		virtual ~MonoDE2_GlobalSearch(){}
		bool init(const vector<string> &params);
		void getSolution(MOFront *p);
		void printInfo(ostream &os) const ;
	private:
		void variation(int candidate);
		void printBestIndividual();
		void runGeneration();
		void replacement();

    void globalSearch();
    // Global Search original
    void globalSearchGlobalNeighbourhood();
    void globalSearchGlobalNeighbourhood2();
    void globalSearchGlobalNeighbourhood3();
    void globalSearchGlobalNeighbourhood3_2();
    void globalSearchGlobalNeighbourhood4();
    void globalSearchGlobalNeighbourhood5();
    void globalSearchGlobalNeighbourhood5_2();
    void globalSearchGlobalNeighbourhood6();
    // Global Neighbourhood Path Search (Used in GENOPT contest)
    void globalSearchGlobalNeighbourhood6_2();
    void globalSearchGlobalNeighbourhood6_3();
    void globalSearchGlobalNeighbourhood7();
    void globalSearchGlobalNeighbourhood7_2();
    // Similarity-based Global Search (SGS): Linear descending function
    void globalSearchGlobalNeighbourhood8();
    // Similarity-based Global Search (SGS): Linear ascending function
    void globalSearchGlobalNeighbourhood9();
    // Similarity-based Global Search (SGS): Considers variability of the population and a threshold value
    void globalSearchGlobalNeighbourhood10();
    // Similarity-based Global Search (SGS): Equal to 8() but without "while (improvement)" loop
    void globalSearchGlobalNeighbourhood11();
    // Similarity-based Global Search (SGS): After COAP reviews
    void globalSearchGlobalNeighbourhood12();

    // Sorts the population in ascending/descending order by considering the Euclidean distance in the
    // decision space to the fittest individual
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

    // Vector which includes the population of extended individuals 
    vector<extIndType*> *auxPop;
    // Sorts the population (extended individuals) in ascending/descending order by considering the 
    // Euclidean distance in the decision space to the fittest individual
    bool sortByDistanceToBestExtInd(const extIndType *i1, const extIndType *i2, const Individual *best);
    bool sortByDistanceToBestExtIndReversed(const extIndType *i1, const extIndType *i2, const Individual *best);

		// Nelder Mead
    void NelderMeadSelect();
    void NelderMead();
    void NelderMeadNoShrink();
    void NelderMeadAdaptive();

		// Opposition-based Learning with jumps
    void jumping();
    bool jumping_QOBL();
    bool jumping_QROBL();

		void updateNuF();
		void updateNuCR();
		void generateF(int candidate);
		void generateCR(int candidate);
		void mutate(Individual *trial, int r1, int r2, int r3, int bestp, int candidate, int var);
		void updateCRmSADE();
		void fillPopWithRestart();
		void fillPopWithMersenneTwister();
		void fillPopWithTentMap();
		void fillPopWithSobol();
		void fillPopWithGoodLatticePoints();
		void fillPopWithOppositionBasedLearning();
		void fillPopWithQuasiOppositionBasedLearning();
		void fillPopWithQuasiReflectedOppositionBasedLearning();
		void fillPopWithNewIndsAndEvaluate();
		
		vector<Individual *> offspring;
		int ps;//population size
		int seed;//seed for the population initialization techniques
		double nuF, nuCR, p, c;
		double FFixed;
		double CRFixed;
		double FnormalNu, FnormalSigma;
		double CRnormalNu, CRnormalSigma;
		double CRcauchyNu, CRcauchyLocFactor;
		double jDE_Fmin, jDE_Fmax, jDE_tauF;
		double jDE_CRmin, jDE_CRmax, jDE_tauCR;
    double minPopPercentage;
    int maxVarValues;
    double varThreshold;
    vector<double> varValues;
		int LP; //Learning period in SaDE
		int CRMemSize;//Archive size if SaDE
		vector<double> fValues, crValues;
		vector<int> fValuesCompetitiveIndex, crValuesCompetitiveIndex;
		vector<double> currentFValues, currentCRValues;//jDE
		vector<double> usedCRValues;
		vector<double> competitiveFs, competitiveCRs;
		vector<int> competitiveFsCount, competitiveCRsCount;
		double fIncr;
		vector<double> successfullFs, successfullCRs, lastImprovements;
		list<double> successfullCRsQueue;
		bool updateNuCRRequired, updateNuFRequired, globalSearchRequired, nelderMeadRequired, jumpingRequired;
    double prevGenBestFitness;
		int popInitType, reinitializationType, fType, updateNuFStrategy, trialStrategy, crossover, crType, updateNuCRStrategy, globalSearchType, nelderMeadType, jumpingType;
		double competitive_fMin, competitive_fMax, competitive_fdelta;
		double competitive_crMin, competitive_crMax, competitive_crdelta;
		double CRMin_uni, CRMax_uni;
		double CRBin, CRExp;
    double jumpingRate;
		int competitive_fN0, competitive_crN0;
		int competitive_fCount, competitive_crCount;
		int replacements;
};

#endif
