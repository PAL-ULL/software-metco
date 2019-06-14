/*
 * @Author: Alejandro Marrero 
 * @Contact: alu0100825008@ull.edu.es 
 * @Date: 2018-10-13 09:15:09 
 *
 *	Single Objective Healthy Menu
 *	Code for the CEC 2019 paper
 */


#ifndef __SOHM_H__
#define __SOHM_H__

#include "Individual.h"


class SOHM: public Individual {
public:
	// Constructor and destructor
  	SOHM() {};
  	~SOHM() {};
	// Initialization method
	bool init (const vector<string> &params);
	// Evaluation
	void evaluate (void);
	// Clone
	Individual* clone (void) const;
  	// Ranges
	double getMaximum(const int i) const { return 0; };
	double getMinimum(const int i) const { return 0; };
  	unsigned int getOptDirection(const int i) const { return MINIMIZE; };
};

#endif
