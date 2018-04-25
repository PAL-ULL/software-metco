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


#ifndef MMBO_H
#define MMBO_H

#include "Individual.h"
#include "EA.h"

#include <vector>
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <math.h>
#include <functional>

using namespace std;

class MultiLeadMigBirds : public EA {

	public:
		bool init(const vector<string> &params);
		void getSolution(MOFront *p);
		void printInfo(ostream &os) const;
	private:
		void runGeneration();
    void determineFlock();
    void flyFlock();
    Individual* lookForBestInd();
    Individual* lookForWorstInd();

    // Attributes
    int noi, nob, non, olf;
    int contLeaders, contNoLeaders;
    bool leaderImproves;
    vector<int> noLeaders;
    vector< vector<int> > leaders;
    vector< vector<Individual*> > neighborSets;
    vector<Individual*> bests;

    // Methods for manipulating neighbor sets
    void createNeighborSet(int ind, int nongbr);
    Individual* neighbor(int ind);
    Individual* neighbor2(int ind);
    Individual* neighborDE(int ind);
    Individual* neighborDE2(int ind);
    Individual* neighborSphere(int ind);
    Individual* getBestNeighbor(int ind, bool remove = false);
    void addNeighbor(int ind, Individual *aux);
};

bool sortByObj(const Individual *i1, const Individual *i2);

#endif
