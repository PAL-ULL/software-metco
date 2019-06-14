/***********************************************************************************
 * AUTHORS
 *  - Eduardo Segredo Gonzalez
 * 
 * DATE
 *    April 2016
 *
 **********************************************************************************/


#ifndef MBO_H
#define MBO_H

#include "Individual.h"
#include "EA.h"

#include <vector>
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <math.h>
#include <functional>

using namespace std;

class MigBirdsOpt : public EA {

	public:
		bool init(const vector<string> &params);
    void getSolution(MOFront *p);
		void printInfo(ostream &os) const;
	private:
		void runGeneration();
    void flyFlock();
	  void replaceLeaderWithSuccessor();
    Individual* lookForBestInd();
    Individual* lookForWorstInd();

    // Attributes
    int noi, nob, non, olf, nof;
    int contLeaders, contNoLeaders;
    bool leftSide;
    vector<int> noLeaders;
    vector< vector<int> > leaders;
    vector< vector<Individual*> > neighborSets;
    vector<Individual*> bests;

    // Methods for manipulating neighbor sets
    void createNeighborSet(int ind, int nongbr);
    Individual* neighbor(int ind);
    Individual* neighbor2(int ind);
    Individual* neighborSphere(int ind);
    Individual* neighborDE(int ind);
    Individual* getBestNeighbor(int ind, bool remove = false);
    void addNeighbor(int ind, Individual *aux);
};

bool sortByObj(const Individual *i1, const Individual *i2);

#endif
