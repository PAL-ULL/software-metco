/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 01/11/13
* Descripcion: Differential Evolution sin la parte de HMR que tiene MonoDE
* con el fin de que el codigo sea mas simple
************************************************************************/

#ifndef _JADE_H_
#define _JADE_H_

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

using namespace std;

// Initialization Types
// Variable popInitType
#define POP_INIT_RESTART 1
#define POP_INIT_MERSENNE_TWISTER 2
#define POP_INIT_TENT_MAP 3
#define POP_INIT_SOBOL 4
#define POP_INIT_LATTICE 5
#define POP_INIT_OPPOSITION_BASED_LEARNING 6
#define POP_INIT_QUASI_OPPOSITION_BASED_LEARNING 7

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

class MonoDE2 : public EA {
	public:
		virtual ~MonoDE2(){}
		bool init(const vector<string> &params);
		void getSolution(MOFront *p);
		void printInfo(ostream &os) const ;
	private:
		void variation(int candidate);
		void printBestIndividual();
		void runGeneration();
		void replacement();
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
		bool updateNuCRRequired, updateNuFRequired;
		int popInitType, reinitializationType, fType, updateNuFStrategy, trialStrategy, crossover, crType, updateNuCRStrategy;
		double competitive_fMin, competitive_fMax, competitive_fdelta;
		double competitive_crMin, competitive_crMax, competitive_crdelta;
		double CRMin_uni, CRMax_uni;
		double CRBin, CRExp;
		int competitive_fN0, competitive_crN0;
		int competitive_fCount, competitive_crCount;
		int replacements;
};

#endif
