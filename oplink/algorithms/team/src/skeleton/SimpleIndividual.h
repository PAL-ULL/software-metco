/***********************************************************************************
 * AUTORES
 *   - Carlos Segura González
 * 
 * FECHA
 *    Mayo 2010 
 *
 * ********************************************************************************/

#ifndef __SIMPLE_INDIVIDUAL_H__
#define __SIMPLE_INDIVIDUAL_H__

#include "Individual.h"
#include <math.h>

class SimpleIndividual : public Individual {
	public:
		SimpleIndividual() {}
		SimpleIndividual(int nVar, int nObj) { this->setNumberOfVar(nVar); this->setNumberOfObj(nObj); optDirections.insert(optDirections.begin(), nObj, MINIMIZE); }
		virtual ~SimpleIndividual() {}
	
		// Evaluation
		void evaluate (void);
	
		// Ranges
		double getMaximum(const int i) const { cout << "It shouldn't be executed never --> SimpleIndividual::getMaximum" << endl; return 1; }
		double getMinimum(const int i) const { cout << "It shouldn't be executed never --> SimpleIndividual::getMinimum" << endl; return 0; }
		unsigned int getOptDirection(const int i) const { return optDirections[i]; }
		void setOptDirection(const int i, const int d) { optDirections[i] = d; }
	
		// Clone
		Individual* clone (void) const;
	
	private:
		vector<double> optDirections;
};

#endif
