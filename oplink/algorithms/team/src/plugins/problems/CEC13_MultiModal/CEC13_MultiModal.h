#ifndef _CEC13_MULTIMODAL_H
#define _CEC13_MULTIMODAL_H

using namespace std;

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

#include "Individual.h"
#include "cec2013.h"

class CEC13_MultiModal:public Individual {
public:

	// Initialization method
	bool init(const vector<string> &params);

	// Evaluation
	void evaluate();

	// Ranges
	double inline getMaximum(const int i) const;
	double inline getMinimum(const int i) const;
	unsigned int inline getOptDirection(const int i) const { return MAXIMIZE; }

	// Clone
	Individual* clone() const;

  // Getters and setters
  CEC2013* getFunc() const { return func; }
  void inline setFunc(CEC2013 *func) { this->func = func; }

private:

  // CEC2013_MultiModal library problem
  static CEC2013* func;
};

#endif
