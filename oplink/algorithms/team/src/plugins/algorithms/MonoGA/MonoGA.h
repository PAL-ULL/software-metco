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


#ifndef MONOGA_H
#define MONOGA_H

#include "Individual.h"
#include "EA.h"

#include <vector>

using namespace std;

#define SURVIVAL_SELECTION_SS 0
#define SURVIVAL_SELECTION_GENERATIONAL 1
#define SURVIVAL_SELECTION_REPLACEWORST_FROMPARENTANDOFFSPRING 2

class MonoGA : public EA {

	public:
		bool init(const vector<string> &params);
		void getSolution(MOFront *p);
		void printInfo(ostream &os) const;
	private:
		void runGeneration();
		void evaluateOffspring();
		void sortPopulation();
		void binaryTournamentSelection(int &p1, int &p2);
		void parentSelection(int &p1, int &p2);
		void createOffSpring();
		void createOffSprings();
		void survivalSelection();
		
		vector<Individual *> offSpring;
		double pm, pc;
		int survivalSelectionType;
};

bool sortByObj0(const Individual *i1, const Individual *i2);

#endif
