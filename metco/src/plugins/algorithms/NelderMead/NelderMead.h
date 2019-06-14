/***********************************************************************************
 * AUTHORS
 *  - Eduardo Segredo Gonzalez
 * 
 * DATE
 *    March 2016
 *
 * DESCRIPTION
 *		Nelder-Mead Method
 *     
 **********************************************************************************/


#ifndef NELDERMEAD_H
#define NELDERMEAD_H

#include "Individual.h"
#include "EA.h"

#include <vector>

using namespace std;

class NelderMead : public EA {

	public:
		bool init(const vector<string> &params);
		void getSolution(MOFront *p);
		void printInfo(ostream &os) const;
	private:
		void runGeneration();
		void sortPopulation();

    double nm1, nm2, nm3, nm4;
    int dimension;
};

bool sortByObj0(const Individual *i1, const Individual *i2);

#endif
